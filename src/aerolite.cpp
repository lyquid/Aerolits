#include "include/aerolite.hpp"
#include "include/box2d_utils.hpp"
#include "include/game.hpp"
#include "include/game_entity.hpp"
#include "include/paths.hpp"
#include "include/random.hpp"
#include "kuge/kuge.hpp"
#include "sdl2_wrappers/sdl2_log.hpp"
#include "sdl2_wrappers/sdl2_renderer.hpp"
#include "sdl2_wrappers/sdl2_texture.hpp"
#include <limits>

/* GRAPHICS */

ktp::ShaderProgram ktp::AeroliteGraphicsComponent::shader_ {};

ktp::AeroliteGraphicsComponent::AeroliteGraphicsComponent() {
  // generateOpenGLStuff(ConfigParser::aerolites_config.size_ * kMetersToPixels); ???????
  const auto vertex_shader_path {getResourcesPath("shaders") + "aerolite.vert"};
  const auto fragment_shader_path {getResourcesPath("shaders") + "aerolite.frag"};
  // shader_.setup(vertex_shader_path, fragment_shader_path);
}

void ktp::AeroliteGraphicsComponent::update(const GameEntity& aerolite) {
  shader_.use();
  vao_.bind();
  // glDraw...
}

/* PHYSICS */

ktp::AerolitePhysicsComponent::AerolitePhysicsComponent(GameEntity* owner, AeroliteGraphicsComponent* graphics) noexcept:
 graphics_(graphics) {
  born_time_ = Game::gameplay_timer_.milliseconds();
  owner_ = owner;
  size_ = ConfigParser::aerolites_config.size_.value_ * generateRand(ConfigParser::aerolites_config.size_.rand_min_, ConfigParser::aerolites_config.size_.rand_max_);
  //generateAeroliteShape(shape_, size_);
  //graphics_->renderShape().resize(shape_.size() + 1);
  createB2Body(*this);
}

ktp::AerolitePhysicsComponent::AerolitePhysicsComponent(GameEntity* owner, AeroliteGraphicsComponent* graphics, float size) noexcept:
 graphics_(graphics) {
  born_time_ = Game::gameplay_timer_.milliseconds();
  owner_ = owner;
  size_ = size;
  //generateAeroliteShape(shape_, size_);
  //graphics_->renderShape().resize(shape_.size() + 1);
  createB2Body(*this);
}

ktp::AerolitePhysicsComponent& ktp::AerolitePhysicsComponent::operator=(AerolitePhysicsComponent&& other) noexcept {
  if (this != &other) {
    // inherited members
    body_     = std::exchange(other.body_, nullptr);
    collided_ = other.collided_;
    delta_    = std::move(other.delta_);
    owner_    = std::exchange(other.owner_, nullptr);
    //shape_    = std::move(other.shape_);
    size_     = other.size_;
    // own members
    graphics_       = std::exchange(other.graphics_, nullptr);
    aabb_           = std::move(other.aabb_);
    born_time_      = other.born_time_;
    new_born_       = other.new_born_;
    world_manifold_ = std::move(other.world_manifold_);
  }
  return *this;
}

void ktp::AerolitePhysicsComponent::createB2Body(AerolitePhysicsComponent& aerolite) {
  if (aerolite.body_) world_->DestroyBody(aerolite.body_);

  b2BodyDef body_def {};
  body_def.type = b2_dynamicBody;
  body_def.userData.pointer = reinterpret_cast<uintptr_t>(aerolite.owner_);
  aerolite.body_ = world_->CreateBody(&body_def);

  std::vector<B2Vec2Vector> fixtures_shapes {};
  //Geometry::triangulate(aerolite.shape_, fixtures_shapes);
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

void ktp::AerolitePhysicsComponent::generateAeroliteShape(B2Vec2Vector& shape, float size) {
  generateAeroliteShape(shape, size, generateRand(kMinSides_, kMaxSides_));
}

void ktp::AerolitePhysicsComponent::generateAeroliteShape(B2Vec2Vector& shape, float size, unsigned int sides) {
  shape.clear();
  b2Vec2 point {};
  for (auto i = 0u; i < sides; ++i) {
    const auto f_size {size * generateRand(0.8f, 1.f)};
    point.x = f_size * SDL_cosf(2 * b2_pi * i / sides);
    point.y = f_size * SDL_sinf(2 * b2_pi * i / sides);
    shape.push_back(point);
  }
  shape.shrink_to_fit();
}

void ktp::AerolitePhysicsComponent::reshape(float size) {
  size_ = size;
  // generateAeroliteShape(shape_, size_, shape_.size());
  // graphics_->renderShape().clear();
  // graphics_->renderShape().resize(shape_.size() + 1);
  // Box2D
  const auto old_angle   {body_->GetAngle()};
  const auto old_angular {body_->GetAngularVelocity()};
  const auto old_delta   {body_->GetLinearVelocity()};
  const auto old_pos     {body_->GetPosition()};
  createB2Body(*this);
  body_->SetAngularVelocity(old_angular);
  body_->SetLinearVelocity(old_delta);
  body_->SetTransform(old_pos, old_angle);
}

ktp::GameEntity* ktp::AerolitePhysicsComponent::spawnAerolite(const b2Vec2& where) {
  const auto aerolite {GameEntity::createEntity(EntityTypes::Aerolite)};
  if (!aerolite) return nullptr;
  aerolite->physics()->body()->SetTransform(where, 0);
  return aerolite;
}

ktp::GameEntity* ktp::AerolitePhysicsComponent::spawnMovingAerolite() {
  const auto aerolite {GameEntity::createEntity(EntityTypes::Aerolite)};
  if (!aerolite) return nullptr;
  static int side {};
  aerolite->physics()->body()->SetAngularVelocity(ConfigParser::aerolites_config.rotation_speed_.value_ * generateRand(ConfigParser::aerolites_config.rotation_speed_.rand_min_, ConfigParser::aerolites_config.rotation_speed_.rand_max_));
  const float delta {ConfigParser::aerolites_config.speed_.value_ * generateRand(ConfigParser::aerolites_config.speed_.rand_min_, ConfigParser::aerolites_config.speed_.rand_max_)};
  switch (side) {
    case 0: // up
      aerolite->physics()->body()->SetTransform({b2_screen_size_.x * 0.5f, -b2_screen_size_.y}, aerolite->physics()->body()->GetAngle());
      aerolite->physics()->body()->SetLinearVelocity({0, delta});
      ++side;
      break;
    case 1: // right
      aerolite->physics()->body()->SetTransform({b2_screen_size_.x + b2_screen_size_.y, b2_screen_size_.y * 0.5f}, aerolite->physics()->body()->GetAngle());
      aerolite->physics()->body()->SetLinearVelocity({-delta, 0});
      ++side;
      break;
    case 2: // down
      aerolite->physics()->body()->SetTransform({b2_screen_size_.x * 0.5f, b2_screen_size_.y + b2_screen_size_.y}, aerolite->physics()->body()->GetAngle());
      aerolite->physics()->body()->SetLinearVelocity({0, -delta});
      ++side;
      break;
    case 3: // left
      aerolite->physics()->body()->SetTransform({-b2_screen_size_.y, b2_screen_size_.y * 0.5f}, aerolite->physics()->body()->GetAngle());
      aerolite->physics()->body()->SetLinearVelocity({delta, 0});
      side = 0;
      break;
  }
  return aerolite;
}

void ktp::AerolitePhysicsComponent::split() {
  if (size_ < kMinSize_) {
    // very small, destroyed on impact
    owner_->deactivate();
    kuge::AeroliteDestroyedEvent ev {
      kuge::KugeEventTypes::AeroliteDestroyed,
      {owner_->physics()->body()->GetPosition().x * kMetersToPixels,
       owner_->physics()->body()->GetPosition().y * kMetersToPixels},
       (int)(kScore_ / size_)};
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
    const GameEntity* aerolite {nullptr};
    for (std::size_t i = 0; i < pieces; ++i) {
      aerolite = GameEntity::createEntity(EntityTypes::Aerolite);
      if (!aerolite) return;
      static_cast<AerolitePhysicsComponent*>(aerolite->physics())->new_born_ = false;
      static_cast<AerolitePhysicsComponent*>(aerolite->physics())->reshape(piece_size); // need this until we can use the size constructor
      aerolite->physics()->body()->SetAngularVelocity(old_angular * generateRand(-1.5f, 1.5f));
      aerolite->physics()->body()->SetLinearVelocity({old_delta.x * generateRand(0.5f, 1.5f), old_delta.y * generateRand(0.5f, 1.5f)});
      where.x = perpendicular.end.x + kSpacer * (perpendicular.begin.x - perpendicular.end.x);
      where.y = perpendicular.end.y + kSpacer * (perpendicular.begin.y - perpendicular.end.y);
      aerolite->physics()->body()->SetTransform({where.x, where.y}, old_angle);
    }

    kuge::AeroliteSplittedEvent ev {
      kuge::KugeEventTypes::AeroliteSplitted,
      {old_pos.x * kMetersToPixels, old_pos.y * kMetersToPixels},
      pieces,
      (int)(((kScore_ / 10u) / size_) * (pieces + 1u))
    };
    owner_->event_bus_->postEvent(ev);
  }
}

void ktp::AerolitePhysicsComponent::transformRenderShape() {
  // for (auto i = 0u; i < shape_.size(); ++i) {
    // graphics_->renderShape().data()[i].x = ((shape_[i].x * SDL_cosf(body_->GetAngle()) - shape_[i].y * SDL_sinf(body_->GetAngle())) + body_->GetPosition().x) * kMetersToPixels;
    // graphics_->renderShape().data()[i].y = ((shape_[i].x * SDL_sinf(body_->GetAngle()) + shape_[i].y * SDL_cosf(body_->GetAngle())) + body_->GetPosition().y) * kMetersToPixels;
  // }
  // graphics_->renderShape().back() = graphics_->renderShape().front();
}

void ktp::AerolitePhysicsComponent::update(const GameEntity& aerolite, float delta_time) {
  if (collided_) {
    new_born_ = false;
    collided_ = false;
    split();
  }

  transformRenderShape();

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
}
