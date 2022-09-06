#include "include/aerolite.hpp"
#include "include/camera.hpp"
#include "include/game.hpp"
#include "include/game_entity.hpp"
#include "include/random.hpp"
#include "kuge/kuge.hpp"
#include "sdl2_wrappers/sdl2_log.hpp"
#include <algorithm>
#include <limits>

/* GRAPHICS */

ktp::AeroliteGraphicsComponent::AeroliteGraphicsComponent() {
  shader_.use();
  shader_.setFloat4("aerolite_color", glm::value_ptr(color_));
}

void ktp::AeroliteGraphicsComponent::update(const GameEntity& aerolite) {
  shader_.use();
  shader_.setMat4f("mvp", glm::value_ptr(mvp_));
  texture_.bind();
  vao_.bind();
  glDrawElements(GL_TRIANGLES, (int)indices_count_, GL_UNSIGNED_INT, 0);
}

/* PHYSICS */

ktp::AerolitePhysicsComponent::AerolitePhysicsComponent(GameEntity* owner, AeroliteGraphicsComponent* graphics): graphics_(graphics) {
  born_time_ = Game::gameplay_timer_.milliseconds();
  owner_ = owner;
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
  graphics_->uv_.setup(texture_coords);
  // convert box2d coords to pixels and set up the vertices VBO
  std::transform(triangulated_shape.begin(), triangulated_shape.end(), triangulated_shape.begin(), [](auto coord){return coord * kMetersToPixels;});
  graphics_->vertices_.setup(triangulated_shape);
  // link the attributes
  graphics_->vao_.linkAttrib(graphics_->vertices_, 0, 3, GL_FLOAT, 0, nullptr);
  graphics_->vao_.linkAttrib(graphics_->uv_,       1, 2, GL_FLOAT, 0, nullptr);
  // setup the EBO
  graphics_->indices_count_ = indices.size();
  graphics_->ebo_.setup(indices);
  // the pointing arrow
  arrow_ = static_cast<AeroliteArrowPhysicsComponent*>(GameEntity::createEntity(EntityTypes::AeroliteArrow)->physics());
}

ktp::AerolitePhysicsComponent::~AerolitePhysicsComponent() {
  if (arrow_) arrow_->owner()->deactivate();
  if (body_) world_->DestroyBody(body_);
}

ktp::AerolitePhysicsComponent& ktp::AerolitePhysicsComponent::operator=(AerolitePhysicsComponent&& other) {
  if (this != &other) {
    // inherited members
    collided_ = other.collided_;
    delta_    = std::move(other.delta_);
    owner_    = std::exchange(other.owner_, nullptr);
    size_     = other.size_;
    // own members
    arrow_          = std::exchange(other.arrow_, nullptr);
    arrow_needed_   = other.arrow_needed_;
    graphics_       = std::exchange(other.graphics_, nullptr);
    aabb_           = std::move(other.aabb_);
    body_           = std::exchange(other.body_, nullptr);
    born_time_      = other.born_time_;
    new_born_       = other.new_born_;
    world_manifold_ = std::move(other.world_manifold_);
  }
  return *this;
}

ktp::GLfloatVector ktp::AerolitePhysicsComponent::convertToUV(const GLfloatVector& v) {
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

void ktp::AerolitePhysicsComponent::createB2Body(AerolitePhysicsComponent& aerolite, const GLfloatVector& triangulated_shape) {
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

ktp::Geometry::Polygon ktp::AerolitePhysicsComponent::generateAeroliteShape(float size, SDL_FPoint offset) {
  return generateAeroliteShape(size, generateRand(kMinSides_, kMaxSides_), offset);
}

ktp::Geometry::Polygon ktp::AerolitePhysicsComponent::generateAeroliteShape(float size, unsigned int sides, SDL_FPoint offset) {
  Geometry::Polygon shape {};
  shape.reserve(sides);
  GLfloat x {}, y {};
  // Generates a shape around coord [0,0], CCW, length based on the size specified
  for (auto i = 0u; i < sides; ++i) {
    const auto f_size {size * generateRand(0.82f, 1.f)};
    x = f_size * SDL_cosf(2.f * b2_pi * (float)i / (float)sides) + offset.x;
    y = f_size * SDL_sinf(2.f * b2_pi * (float)i / (float)sides) + offset.y;
    shape.push_back({x, y});
  }
  return shape;
}

bool ktp::AerolitePhysicsComponent::intersectPoint(const glm::vec2& a, const glm::vec2& b, const glm::vec2& c, const glm::vec2& d, glm::vec2& result) {
  // Line AB represented as a1x + b1y = c1
  const auto a1 {b.y - a.y};
  const auto b1 {a.x - b.x};
  const auto c1 {a1 * a.x + b1 * a.y};
  // Line CD represented as a2x + b2y = c2
  const auto a2 {d.y - c.y};
  const auto b2 {c.x - d.x};
  const auto c2 {a2 * c.x + b2 * c.y};

  const auto determinant {a1 * b2 - a2 * b1};

  if (determinant == 0.f) { // ugly comparison
    //paralel lines
    return false;
  }
  else {
    result.x = (b2 * c1 - b1 * c2) / determinant;
    result.y = (a1 * c2 - a2 * c1) / determinant;
    return true;
  }
}

void ktp::AerolitePhysicsComponent::reshape(float size) {
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

ktp::GameEntity* ktp::AerolitePhysicsComponent::spawnAerolite(const b2Vec2& where) {
  const auto aerolite {static_cast<AerolitePhysicsComponent*>(GameEntity::createEntity(EntityTypes::Aerolite)->physics())};
  if (!aerolite) return nullptr;
  aerolite->body_->SetTransform({where.x * kPixelsToMeters, where.y * kPixelsToMeters}, 0);
  return aerolite->owner();
}

ktp::GameEntity* ktp::AerolitePhysicsComponent::spawnMovingAerolite() {
  const auto spawner {static_cast<AeroliteSpawnerPhysicsComponent*>(GameEntity::findFirstOf(EntityTypes::AeroliteSpawner)->physics())};
  // check if spawner maybe touching something
  if (!spawner || spawner->maybeTouching()) return nullptr;
  const auto aerolite {static_cast<AerolitePhysicsComponent*>(GameEntity::createEntity(EntityTypes::Aerolite)->physics())};
  // check if we are allowed to create a new entity
  if (!aerolite) return nullptr;
  // all good to go!
  const auto spawn_data {*spawner->spawnPoint()};
  // now we have to find the direction we want the aerolite to go (towards the center)
  // the directional vector can be determined by subtracting the start from the terminal point
  const glm::vec2 direction {glm::normalize(spawn_data.start_point_ - spawn_data.end_point_)};
  // linear velocity
  const auto linear_speed {ConfigParser::aerolites_config.speed_.value_ * generateRand(ConfigParser::aerolites_config.speed_.rand_min_, ConfigParser::aerolites_config.speed_.rand_max_)};
  aerolite->body_->SetLinearVelocity(-linear_speed * b2Vec2{direction.x, direction.y});
  // angular velocity
  aerolite->body_->SetAngularVelocity(ConfigParser::aerolites_config.rotation_speed_.value_ * generateRand(ConfigParser::aerolites_config.rotation_speed_.rand_min_, ConfigParser::aerolites_config.rotation_speed_.rand_max_));
  // position
  aerolite->body_->SetTransform(b2Vec2{spawn_data.start_point_.x, spawn_data.start_point_.y}, aerolite->body_->GetAngle());
  // another point of the aerolite trajectory
  const glm::vec2 aerolite_pos2 {
    spawn_data.start_point_.x + aerolite->body_->GetLinearVelocity().x,
    spawn_data.start_point_.y + aerolite->body_->GetLinearVelocity().y
  };
  // where the aerolite will cross the screen
  glm::vec2 intersect_point {};
  // arrow: calculate the angle of attack respect to the screen's center
  const auto theta {atan2Normalized(spawn_data.start_point_.y - b2_screen_size_.y * 0.5f, spawn_data.start_point_.x - b2_screen_size_.x * 0.5f)};
  // asign an incoming direction so we can position the arrow
  if (theta >= top_right && theta < top_left) {
    // TOP
    aerolite->arrow_->incoming_direction_ = Direction::Top;
    intersectPoint(spawn_data.start_point_, aerolite_pos2, {0.f, b2_screen_size_.y}, {b2_screen_size_.x, b2_screen_size_.y}, intersect_point);
  } else if (theta >= top_left && theta < bottom_left) {
    // LEFT
    aerolite->arrow_->incoming_direction_ = Direction::Left;
    intersectPoint(spawn_data.start_point_, aerolite_pos2, {0.f, 0.f}, {0.f, b2_screen_size_.y}, intersect_point);
  } else if (theta >= bottom_left && theta < bottom_right) {
    // BOTTOM
    aerolite->arrow_->incoming_direction_ = Direction::Bottom;
    intersectPoint(spawn_data.start_point_, aerolite_pos2, {0.f, 0.f}, {b2_screen_size_.x, 0.f}, intersect_point);
  } else {
    // RIGHT
    aerolite->arrow_->incoming_direction_ = Direction::Right;
    intersectPoint(spawn_data.start_point_, aerolite_pos2, {b2_screen_size_.x, 0.f}, {b2_screen_size_.x, b2_screen_size_.y}, intersect_point);
  }
  // distance from spawn point to intersect point
  const auto distance {glm::distance(spawn_data.start_point_, intersect_point)};
  // calculate the time for the aerolite to enter the screen, minus 5% to compensate for the aerolite radius
  aerolite->arrow_->time_to_enter_ = (distance / aerolite->body_->GetLinearVelocity().Length()) * 0.95f;

  return aerolite->owner();
}

void ktp::AerolitePhysicsComponent::positionArrow() {
  const glm::vec2 aerolite_position {body_->GetPosition().x * kMetersToPixels, body_->GetPosition().y * kMetersToPixels};
  const glm::vec2 screen {b2_screen_size_.x * kMetersToPixels, b2_screen_size_.y * kMetersToPixels};
  constexpr auto size {AeroliteArrowGraphicsComponent::kSize_ / 2.f};
  switch (arrow_->incoming_direction_) {
    case Direction::Top:
      if (aerolite_position.x < size) {
        arrow_->position_ = glm::vec3(size, screen.y - size, 0.f);
      } else if (aerolite_position.x > screen.x - size) {
        arrow_->position_ = glm::vec3(screen.x - size, screen.y - size, 0.f);
      } else {
        arrow_->position_ = glm::vec3(aerolite_position.x, screen.y - size, 0.f);
      }
      break;
    case Direction::Left:
      if (aerolite_position.y < size) {
        arrow_->position_ = glm::vec3(size, size, 0.f);
      } else if (aerolite_position.y > screen.y - size) {
        arrow_->position_ = glm::vec3(size, screen.y - size, 0.f);
      } else {
        arrow_->position_ = glm::vec3(size, aerolite_position.y, 0.f);
      }
      break;
    case Direction::Bottom:
      if (aerolite_position.x < size) {
        arrow_->position_ = glm::vec3(size, size, 0.f);
      } else if (aerolite_position.x > screen.x - size) {
        arrow_->position_ = glm::vec3(screen.x - size, size, 0.f);
      } else {
        arrow_->position_ = glm::vec3(aerolite_position.x, size, 0.f);
      }
      break;
    case Direction::Right:
      if (aerolite_position.y < size) {
        arrow_->position_ = glm::vec3(screen.x - size, size, 0.f);
      } else if (aerolite_position.y > screen.y - size) {
        arrow_->position_ = glm::vec3(screen.x - size, screen.y - size, 0.f);
      } else {
        arrow_->position_ = glm::vec3(screen.x - size, aerolite_position.y, 0.f);
      }
      break;
    default:
      break;
  }
  // angle of the arrow
  arrow_->angle_ = atan2f(aerolite_position.y - arrow_->position_.y, aerolite_position.x - arrow_->position_.x) - b2_pi * 0.5f;
}

void ktp::AerolitePhysicsComponent::split() {
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
      aerolite->arrow_needed_ = false;
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

void ktp::AerolitePhysicsComponent::update(const GameEntity& aerolite, float delta_time) {
  // if collided remove the new_born status and
  // deactivate the collided flag for the new aerolite
  if (collided_) {
    new_born_ = false;
    collided_ = false;
    split();
  }

  aabb_.lowerBound = b2Vec2{ std::numeric_limits<float>::max(),  std::numeric_limits<float>::max()};
  aabb_.upperBound = b2Vec2{-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max()};
  // combine all the fixtures of the body to make a "big" AABB
  for (b2Fixture* fixture = body_->GetFixtureList(); fixture; fixture = fixture->GetNext()) {
    aabb_.Combine(aabb_, fixture->GetAABB(0));
  }
  constexpr auto kThreshold {0.1f};
  if (!new_born_ && (aabb_.upperBound.x < -kThreshold || aabb_.lowerBound.x > b2_screen_size_.x + kThreshold
                  || aabb_.upperBound.y < -kThreshold || aabb_.lowerBound.y > b2_screen_size_.y + kThreshold)) {
    // aerolite entered and then exit the screen
    owner_->deactivate();
  } else if (new_born_ && aabb_.upperBound.x > -kThreshold && aabb_.lowerBound.x < b2_screen_size_.x
                       && aabb_.upperBound.y > -kThreshold && aabb_.lowerBound.y < b2_screen_size_.y) {
    // aerolite entered the screen
    new_born_ = false;
    arrow_needed_ = false;
    if (arrow_) arrow_->owner()->deactivate();
    arrow_ = nullptr;
  }
  if (new_born_ && Game::gameplay_timer_.milliseconds() - born_time_ > kNewBornTime_) new_born_ = false;

  updateMVP();
  if (arrow_needed_) positionArrow();
}

void ktp::AerolitePhysicsComponent::updateMVP() {
  glm::mat4 model {1.f};
  model = glm::translate(model, glm::vec3(body_->GetPosition().x * kMetersToPixels, body_->GetPosition().y * kMetersToPixels, 0.f));
  model = glm::rotate(model, body_->GetAngle(), glm::vec3(0.f, 0.f, 1.f));
  graphics_->mvp_ = camera_.projectionMatrix() * camera_.viewMatrix() * model;
}

// ARROW GRAPHICS

ktp::AeroliteArrowGraphicsComponent::AeroliteArrowGraphicsComponent() {
  const GLfloatVector arrow_shape {
     0.00f * kSize_,  0.5f * kSize_, 0.f, // top
    -0.25f * kSize_, -0.5f * kSize_, 0.f, // left
     0.25f * kSize_, -0.5f * kSize_, 0.f  // right
  };
  vertices_.setup(arrow_shape);
  // shape
  vao_.linkAttrib(vertices_, 0, 3, GL_FLOAT, 3 * sizeof(GLfloat), nullptr);
}

void ktp::AeroliteArrowGraphicsComponent::update(const GameEntity& aerolite_arrow) {
  shader_.use();
  shader_.setMat4f("mvp", glm::value_ptr(mvp_));
  shader_.setFloat4("color", glm::value_ptr(color_));
  vao_.bind();
  glDrawArrays(GL_TRIANGLES, 0, 3);
}

// ARROW PHYSICS

ktp::AeroliteArrowPhysicsComponent::AeroliteArrowPhysicsComponent(GameEntity* owner, AeroliteArrowGraphicsComponent* graphics): graphics_(graphics) {
  owner_ = owner;
}

ktp::AeroliteArrowPhysicsComponent& ktp::AeroliteArrowPhysicsComponent::operator=(AeroliteArrowPhysicsComponent&& other) {
  if (this != &other) {
    // inherited
    collided_ = other.collided_;
    delta_    = std::move(other.delta_);
    owner_    = std::exchange(other.owner_, nullptr);
    size_     = other.size_;
    // own
    angle_              = other.angle_;
    graphics_           = std::exchange(other.graphics_, nullptr);
    incoming_direction_ = other.incoming_direction_;
    position_           = std::move(other.position_);
    current_color_      = std::move(other.current_color_);
    time_step_          = other.time_step_;
    time_to_enter_      = other.time_to_enter_;
  }
  return *this;
}

void ktp::AeroliteArrowPhysicsComponent::update(const GameEntity& aerolite_arrow, float delta_time) {
  time_step_ += (1.f / time_to_enter_) * delta_time;
  if (time_step_ > 1.f) time_step_ = 1.f;
  current_color_ = Palette::interpolate2Colors(start_color_, end_color_, time_step_);
  graphics_->color_ = current_color_;
  updateMVP();
}

void ktp::AeroliteArrowPhysicsComponent::updateMVP() {
  glm::mat4 model {1.f};
  model = glm::translate(model, position_);
  model = glm::rotate(model, angle_, glm::vec3(0.f, 0.f, 1.f));
  graphics_->mvp_ = camera_.projectionMatrix() * camera_.viewMatrix() * model;
}

// AEROLITE SPAWNER

ktp::AeroliteSpawnerPhysicsComponent::AeroliteSpawnerPhysicsComponent(GameEntity* owner) {
  owner_ = owner;

  b2BodyDef body_def {};
  body_def.type = b2_staticBody;
  body_def.userData.pointer = reinterpret_cast<uintptr_t>(owner_);
  body_ = world_->CreateBody(&body_def);

  b2CircleShape shape {};
  shape.m_radius = 4.f;

  b2FixtureDef fixture_def {};
  fixture_def.shape = &shape;
  fixture_def.isSensor = true;

  body_->CreateFixture(&fixture_def);
  relocateSpawnPoint();
}

ktp::AeroliteSpawnerPhysicsComponent& ktp::AeroliteSpawnerPhysicsComponent::operator=(AeroliteSpawnerPhysicsComponent&& other) {
  if (this != &other) {
    // inherited
    collided_ = other.collided_;
    delta_    = std::move(other.delta_);
    owner_    = std::exchange(other.owner_, nullptr);
    size_     = other.size_;
    // own
    body_                     = std::exchange(other.body_, nullptr);
    maybe_touching_something_ = other.maybe_touching_something_;
    spawn_data_               = std::move(other.spawn_data_);
  }
  return *this;
}

void ktp::AeroliteSpawnerPhysicsComponent::relocateSpawnPoint() {
  const glm::vec2 screen_center {b2_screen_size_.x * 0.5f, b2_screen_size_.y * 0.5f};
  constexpr auto total_spokes {64u};
  const auto spoke {generateRand(0u, total_spokes - 1u)};
  // first we find our spawn_point (start_point)
  spawn_data_.start_point_ = {
    1.5f * b2_screen_size_.x * glm::cos(2.f * b2_pi * (float)spoke / (float)total_spokes) + screen_center.x,
    1.5f * b2_screen_size_.x * glm::sin(2.f * b2_pi * (float)spoke / (float)total_spokes) + screen_center.y
  };
  // move away from the center to create more randomness
  const auto displacement {screen_center.y * generateRand(-1.f, 1.f)};
  spawn_data_.start_point_ += displacement /* + spawn_data_.start_point_ */;
  // a far away point in the line of the trajectory of the aerolite
  spawn_data_.end_point_ = displacement + screen_center;
  // now move the spawner body (the sensor) to the new coords
  body_->SetTransform({spawn_data_.start_point_.x, spawn_data_.start_point_.y}, body_->GetAngle());
}

void ktp::AeroliteSpawnerPhysicsComponent::update(const GameEntity& aerolite_spawner, float delta_time) {
  if (collided_) {
    relocateSpawnPoint();
    maybe_touching_something_ = true;
  } else {
    maybe_touching_something_ = false;
  }
  // this goes false here b/c it seems a good place to do it
  // use maybe_touching_something_ flag to spawn (or not) this tick
  collided_ = false;
}
