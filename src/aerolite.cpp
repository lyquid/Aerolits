#include "include/aerolite.hpp"
#include "include/camera.hpp"
#include "include/game.hpp"
#include "include/random.hpp"
#include "kuge/kuge.hpp"
#include "sdl2_wrappers/sdl2_log.hpp"
#include <algorithm>
#include <limits>

using namespace ktp;

/* MANAGER */

std::vector<AerolitePhysicsComponent> AeroliteManager::aerolite_physics_ {};
std::vector<AeroliteRenderComponent>  AeroliteManager::aerolite_render_ {};
unsigned int AeroliteManager::count_ {0};

void AeroliteManager::activate(EntityId id) {
  aerolite_render_[count_].activate(id);
  aerolite_physics_[count_].activate(id);
  ++count_;
}

void AeroliteManager::deactivate(EntityId id) {
  for (auto i = 0u; i < count_; ++i) {
    bool physics_found {false}, render_found {false};
    if (aerolite_physics_[i].id() == id) {
      aerolite_physics_[i].deactivate();
      physics_found = true;
    }
    if (aerolite_render_[i].id() == id) {
      aerolite_render_[i].deactivate();
      render_found = true;
    }
    if (physics_found && render_found) break;
  }
  --count_;
}

void AeroliteManager::draw() {
  AeroliteRenderComponent::shader().use();
  AeroliteRenderComponent::texture().bind();
  for (auto i = 0u; i < count_; ++i) {
    aerolite_render_[i].update();
  }
}

AerolitePhysicsComponent* AeroliteManager::findPhysics(EntityId id) {
  for (auto i = 0u; i < count_; ++i) {
    if (aerolite_physics_[i].id() == id) return &aerolite_physics_[i];
  }
  return nullptr;
}

AeroliteRenderComponent* AeroliteManager::findRender(EntityId id) {
  for (auto i = 0u; i < count_; ++i) {
    if (aerolite_render_[i].id() == id) return &aerolite_render_[i];
  }
  return nullptr;
}

void AeroliteManager::init() {
  aerolite_physics_.resize(EntityManager::maxEntities());
  aerolite_render_.resize(EntityManager::maxEntities());
}

void AeroliteManager::update(float delta_time) {
  std::partition(aerolite_render_.begin(), aerolite_render_.end(), [](const auto& i){ return i.active(); });
  std::partition(aerolite_physics_.begin(), aerolite_physics_.end(), [](const auto& i){ return i.active(); });
  for (auto i = 0; i < count_; ++i) {
    aerolite_physics_[i].update(delta_time);
  }
}

/* GRAPHICS */

ShaderProgram AeroliteRenderComponent::shader_ {Resources::getShader("aerolite")};
Texture2D     AeroliteRenderComponent::texture_ {Resources::getTexture("aerolite_00")};

AeroliteRenderComponent::AeroliteRenderComponent() {
  shader_.use();
  shader_.setFloat4("aerolite_color", glm::value_ptr(color_)); // can't this be an attribute??
}

void AeroliteRenderComponent::update() {
  shader_.setMat4f("mvp", glm::value_ptr(mvp_));
  vao_.bind();
  glDrawElements(GL_TRIANGLES, indices_count_, GL_UNSIGNED_INT, 0);
}

/* PHYSICS */

AerolitePhysicsComponent::AerolitePhysicsComponent() {
  born_time_ = Game::gameplay_timer_.milliseconds();
  size_ = ConfigParser::aerolites_config.size_.value_ * generateRand(ConfigParser::aerolites_config.size_.rand_min_, ConfigParser::aerolites_config.size_.rand_max_);
  // shape is a vector of points: {[x,y], [x,y], ...} with center at [0,0]
  const auto shape {generateAeroliteShape(size_)};
  // triangulated_shape is a vector of floats: {x, y, z, x, y, z, x, y, ...}
  GLfloatVector triangulated_shape {};
  Geometry::triangulate(shape, triangulated_shape);
  // Box2D
  createB2Body(*this, triangulated_shape);
  // generate a EBO based on the triangulated_shape
  GLuintVector indices {};
  EBO::generateEBO(triangulated_shape, indices);
  // convert cartesian coords to UV coords and set up the uv VBO
  const GLfloatVector texture_coords {convertToUV(triangulated_shape)};
  const auto graphics {AeroliteManager::findRender(id_)}; // DEAD END :( the components don't have the id_ stablished
  graphics_->uv_.setup(texture_coords);
  // convert box2d coords to pixels and set up the vertices VBO
  std::transform(triangulated_shape.begin(), triangulated_shape.end(), triangulated_shape.begin(), [](auto& coord){return coord * kMetersToPixels;});
  graphics_->vertices_.setup(triangulated_shape);
  // link the attributes
  graphics_->vao_.linkAttrib(graphics_->vertices_, 0, 3, GL_FLOAT, 0, nullptr);
  graphics_->vao_.linkAttrib(graphics_->uv_,       1, 2, GL_FLOAT, 0, nullptr);
  // setup the EBO
  graphics_->indices_count_ = indices.size();
  graphics_->ebo_.setup(indices);
}

AerolitePhysicsComponent& AerolitePhysicsComponent::operator=(AerolitePhysicsComponent&& other) {
  if (this != &other) {
    // inherited members
    collided_ = other.collided_;
    delta_    = std::move(other.delta_);
    owner_    = std::exchange(other.owner_, nullptr);
    size_     = other.size_;
    // own members
    graphics_       = std::exchange(other.graphics_, nullptr);
    aabb_           = std::move(other.aabb_);
    body_           = std::exchange(other.body_, nullptr);
    born_time_      = other.born_time_;
    new_born_       = other.new_born_;
    world_manifold_ = std::move(other.world_manifold_);
  }
  return *this;
}

GLfloatVector AerolitePhysicsComponent::convertToUV(const GLfloatVector& v) {
  GLfloat max_x{v[0]}, min_x{v[0]}, max_y{v[1]}, min_y{v[1]};
  for (std::size_t i = 0; i < v.size(); i += 3) {
    if (v[i] > max_x) max_x = v[i];
    else if (v[i] < min_x) min_x = v[i];
    if (v[i + 1] > max_y) max_y = v[i + 1];
    else if (v[i + 1] < min_y) min_y = v[i + 1];
  }
  const auto diagonal_inv {1.f / glm::sqrt((max_x - min_x) * (max_x - min_x) + (max_y - min_y) * (max_y - min_y))};
  GLfloatVector result {};
  result.reserve(static_cast<std::size_t>(v.size() * 0.66f));
  for (std::size_t i = 0; i < v.size(); i += 3) {
    result.push_back((v[i] - min_x) * diagonal_inv);
    result.push_back((v[i + 1] - min_y) * diagonal_inv);
  }
  return result;
}

void AerolitePhysicsComponent::createB2Body(AerolitePhysicsComponent& aerolite, const GLfloatVector& triangulated_shape) {
  if (aerolite.body_) world_->DestroyBody(aerolite.body_);

  b2BodyDef body_def {};
  body_def.type = b2_dynamicBody;
  body_def.userData.pointer = reinterpret_cast<uintptr_t>(aerolite.owner_);
  aerolite.body_ = world_->CreateBody(&body_def);

  std::vector<B2Vec2Vector> fixtures_shapes {};
  for (std::size_t i = 0; i < triangulated_shape.size(); i += 9) {
    B2Vec2Vector vec;
    vec.push_back(b2Vec2{triangulated_shape[i + 0], triangulated_shape[i + 1]});
    vec.push_back(b2Vec2{triangulated_shape[i + 3], triangulated_shape[i + 4]});
    vec.push_back(b2Vec2{triangulated_shape[i + 6], triangulated_shape[i + 7]});
    fixtures_shapes.push_back(vec);
  }

  for (const auto& shape: fixtures_shapes) {
    b2PolygonShape fixture_shape {};
    fixture_shape.Set(shape.data(), shape.size());

    b2FixtureDef fixture_def {};
    fixture_def.shape = &fixture_shape;
    fixture_def.density = ConfigParser::aerolites_config.density_;
    fixture_def.friction = ConfigParser::aerolites_config.friction_;
    fixture_def.restitution = ConfigParser::aerolites_config.restitution_;
    aerolite.body_->CreateFixture(&fixture_def);
  }
}

Geometry::Polygon AerolitePhysicsComponent::generateAeroliteShape(float size, SDL_FPoint offset) {
  return generateAeroliteShape(size, generateRand(kMinSides_, kMaxSides_), offset);
}

Geometry::Polygon AerolitePhysicsComponent::generateAeroliteShape(float size, unsigned int sides, SDL_FPoint offset) {
  Geometry::Polygon shape {};
  shape.reserve(sides);
  GLfloat x {}, y {};
  // Generates a shape around coord [0,0], CCW, length based on the size specified
  for (auto i = 0u; i < sides; ++i) {
    const auto f_size {size * generateRand(0.82f, 1.f)};
    x = f_size * SDL_cosf(2 * b2_pi * i / sides) + offset.x;
    y = f_size * SDL_sinf(2 * b2_pi * i / sides) + offset.y;
    shape.push_back({x, y});
  }
  return shape;
}

void AerolitePhysicsComponent::reshape(float size) {
  size_ = size;
  const auto new_shape {generateAeroliteShape(size_)};
  GLfloatVector triangulated_shape {};
  Geometry::triangulate(new_shape, triangulated_shape);
  // Box2D
  const auto old_angle   {body_->GetAngle()};
  const auto old_angular {body_->GetAngularVelocity()};
  const auto old_delta   {body_->GetLinearVelocity()};
  const auto old_pos     {body_->GetPosition()};
  createB2Body(*this, triangulated_shape);
  body_->SetAngularVelocity(old_angular);
  body_->SetLinearVelocity(old_delta);
  body_->SetTransform(old_pos, old_angle);
  // generate a new EBO based on the triangulated_shape
  GLuintVector indices {};
  EBO::generateEBO(triangulated_shape, indices);
  // normalize texture
  const GLfloatVector texture_coords {convertToUV(triangulated_shape)};
  graphics_->uv_.setup(texture_coords);
  // convert box2d coords to pixels
  std::transform(triangulated_shape.begin(), triangulated_shape.end(), triangulated_shape.begin(), [](auto& coord){return coord * kMetersToPixels;});
  // setup VBO and link the attributes
  graphics_->vertices_.setup(triangulated_shape);
  graphics_->vao_.linkAttrib(graphics_->vertices_, 0, 3, GL_FLOAT, 0, nullptr);
  graphics_->vao_.linkAttrib(graphics_->uv_, 1, 2, GL_FLOAT, 0, nullptr);
  // setup the EBO
  graphics_->indices_count_ = indices.size();
  graphics_->ebo_.setup(indices);
  // change the texture
  // graphics_->texture_ = Resources::getTexture("aerolite_01");
}

GameEntity* AerolitePhysicsComponent::spawnAerolite(const b2Vec2& where) {
  const auto aerolite {static_cast<AerolitePhysicsComponent*>(GameEntity::createEntity(EntityTypes::Aerolite)->physics())};
  if (!aerolite) return nullptr;
  aerolite->body_->SetTransform({where.x * kPixelsToMeters, where.y * kPixelsToMeters}, 0);
  return aerolite->owner();
}

GameEntity* AerolitePhysicsComponent::spawnMovingAerolite() {
  const auto aerolite {static_cast<AerolitePhysicsComponent*>(GameEntity::createEntity(EntityTypes::Aerolite)->physics())};
  if (!aerolite) return nullptr;
  static int side {};
  aerolite->body_->SetAngularVelocity(ConfigParser::aerolites_config.rotation_speed_.value_ * generateRand(ConfigParser::aerolites_config.rotation_speed_.rand_min_, ConfigParser::aerolites_config.rotation_speed_.rand_max_));
  const float delta {ConfigParser::aerolites_config.speed_.value_ * generateRand(ConfigParser::aerolites_config.speed_.rand_min_, ConfigParser::aerolites_config.speed_.rand_max_)};
  switch (side) {
    case 0: // up
      aerolite->body_->SetTransform({b2_screen_size_.x * 0.5f, -b2_screen_size_.y}, aerolite->body_->GetAngle());
      aerolite->body_->SetLinearVelocity({0, delta});
      ++side;
      break;
    case 1: // right
      aerolite->body_->SetTransform({b2_screen_size_.x + b2_screen_size_.y, b2_screen_size_.y * 0.5f}, aerolite->body_->GetAngle());
      aerolite->body_->SetLinearVelocity({-delta, 0});
      ++side;
      break;
    case 2: // down
      aerolite->body_->SetTransform({b2_screen_size_.x * 0.5f, b2_screen_size_.y + b2_screen_size_.y}, aerolite->body_->GetAngle());
      aerolite->body_->SetLinearVelocity({0, -delta});
      ++side;
      break;
    case 3: // left
      aerolite->body_->SetTransform({-b2_screen_size_.y, b2_screen_size_.y * 0.5f}, aerolite->body_->GetAngle());
      aerolite->body_->SetLinearVelocity({delta, 0});
      side = 0;
      break;
  }
  return aerolite->owner();
}

void AerolitePhysicsComponent::split() {
  if (size_ < kMinSize_) {
    // very small, destroyed on impact
    owner_->deactivate();
    kuge::AeroliteDestroyedEvent ev {kuge::KugeEventTypes::AeroliteDestroyed, (int)(kScore_ / size_)};
    owner_->event_bus_->postEvent(ev);
    return;
  } else {
    // good sized aerolite
    const B2Line perpendicular {Geometry::perpendicular(world_manifold_.points[0], body_->GetPosition(), body_->GetPosition())};
    const auto old_angle   {body_->GetAngle()};
    const auto old_angular {body_->GetAngularVelocity()};
    const auto old_delta   {body_->GetLinearVelocity()};
    const auto old_pos     {body_->GetPosition()};
    constexpr unsigned int pieces {1u};
    const float piece_size {size_ * 0.5f};
    constexpr auto kSpacer {0.22f};
    Geometry::Point where {};
    // this Aerolite
    reshape(piece_size);
    where.x = perpendicular.begin.x + kSpacer * (perpendicular.end.x - perpendicular.begin.x);
    where.y = perpendicular.begin.y + kSpacer * (perpendicular.end.y - perpendicular.begin.y);
    body_->SetTransform({where.x, where.y}, old_angle);
    // the new Aerolite
    AerolitePhysicsComponent* aerolite {nullptr};
    for (std::size_t i = 0; i < pieces; ++i) {
      aerolite = static_cast<AerolitePhysicsComponent*>(GameEntity::createEntity(EntityTypes::Aerolite)->physics());
      if (!aerolite) return;
      aerolite->new_born_ = false;
      aerolite->reshape(piece_size); // need this until we can use the size constructor
      aerolite->body_->SetAngularVelocity(old_angular * generateRand(-1.5f, 1.5f));
      aerolite->body_->SetLinearVelocity({old_delta.x * generateRand(0.5f, 1.5f), old_delta.y * generateRand(0.5f, 1.5f)});
      where.x = perpendicular.end.x + kSpacer * (perpendicular.begin.x - perpendicular.end.x);
      where.y = perpendicular.end.y + kSpacer * (perpendicular.begin.y - perpendicular.end.y);
      aerolite->body_->SetTransform({where.x, where.y}, old_angle);
    }
    kuge::AeroliteSplittedEvent ev {
      kuge::KugeEventTypes::AeroliteSplitted,
      pieces,
      (int)(((kScore_ / 10u) / size_) * (pieces + 1u))
    };
    owner_->event_bus_->postEvent(ev);
  }
}

void AerolitePhysicsComponent::update(float delta_time) {
  if (collided_) {
    new_born_ = false;
    collided_ = false;
    split();
  }

  aabb_.lowerBound = b2Vec2{ std::numeric_limits<float>::max(),  std::numeric_limits<float>::max()};
  aabb_.upperBound = b2Vec2{-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max()};

  for (b2Fixture* fixture = body_->GetFixtureList(); fixture; fixture = fixture->GetNext()) {
    aabb_.Combine(aabb_, fixture->GetAABB(0)); /// wtf is childIndex
  }
  constexpr auto kThreshold {0.1f};
  if (!new_born_ && (aabb_.upperBound.x < -kThreshold || aabb_.lowerBound.x > b2_screen_size_.x + kThreshold
                  || aabb_.upperBound.y < -kThreshold || aabb_.lowerBound.y > b2_screen_size_.y + kThreshold)) {
    owner_->deactivate();
  } else if (new_born_ && aabb_.upperBound.x > -kThreshold && aabb_.lowerBound.x < b2_screen_size_.x
                       && aabb_.upperBound.y > -kThreshold && aabb_.lowerBound.y < b2_screen_size_.y) {
    new_born_ = false;
  }
  if (new_born_ && Game::gameplay_timer_.milliseconds() - born_time_ > kNewBornTime_) new_born_ = false;

  updateMVP();
}

void AerolitePhysicsComponent::updateMVP() {
  glm::mat4 model {1.f};
  model = glm::translate(model, glm::vec3(body_->GetPosition().x * kMetersToPixels, body_->GetPosition().y * kMetersToPixels, 0.f));
  model = glm::rotate(model, body_->GetAngle(), glm::vec3(0.f, 0.f, 1.f));
  graphics_->mvp_ = camera_.projectionMatrix() * camera_.viewMatrix() * model;
}
