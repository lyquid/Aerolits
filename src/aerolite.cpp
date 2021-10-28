#include "include/aerolite.hpp"
#include "include/box2d_scale.hpp"
#include "include/game_entity.hpp"
#include "include/random.hpp"
#include "kuge/kuge.hpp"
#include "sdl2_wrappers/sdl2_log.hpp"
#include "sdl2_wrappers/sdl2_renderer.hpp"

/* GRAPHICS */

void ktp::AeroliteGraphicsComponent::update(const GameEntity& aerolite, const SDL2_Renderer& renderer) {
  renderer.setDrawColor(color_);
  renderer.drawLines(render_shape_);
}

/* PHYSICS */

ktp::AerolitePhysicsComponent::AerolitePhysicsComponent(GameEntity* owner, AeroliteGraphicsComponent* graphics) noexcept:
 graphics_(graphics) {
  owner_ = owner;
  size_ = ConfigParser::aerolites_config.size_.value_ * generateRand(ConfigParser::aerolites_config.size_.rand_min_, ConfigParser::aerolites_config.size_.rand_max_);
  generateAeroliteShape(shape_, size_);
  graphics_->renderShape().resize(shape_.size());
  createB2Body(*this);
  body_->SetAngularVelocity(ConfigParser::aerolites_config.rotation_speed_.value_ * generateRand(ConfigParser::aerolites_config.rotation_speed_.rand_min_, ConfigParser::aerolites_config.rotation_speed_.rand_max_));
}

ktp::AerolitePhysicsComponent::AerolitePhysicsComponent(GameEntity* owner, AeroliteGraphicsComponent* graphics, float size) noexcept:
 graphics_(graphics) {
  owner_ = owner;
  size_ = size;
  generateAeroliteShape(shape_, size_);
  graphics_->renderShape().resize(shape_.size());
  createB2Body(*this);
  body_->SetAngularVelocity(ConfigParser::aerolites_config.rotation_speed_.value_ * generateRand(ConfigParser::aerolites_config.rotation_speed_.rand_min_, ConfigParser::aerolites_config.rotation_speed_.rand_max_));
}

ktp::AerolitePhysicsComponent& ktp::AerolitePhysicsComponent::operator=(AerolitePhysicsComponent&& other) noexcept {
  if (this != &other) {
    // inherited members
    body_     = std::exchange(other.body_, nullptr);
    collided_ = other.collided_;
    delta_    = std::move(other.delta_);
    owner_    = std::exchange(other.owner_, nullptr);
    shape_    = std::move(other.shape_);
    size_     = other.size_;
    // own members
    graphics_ = std::exchange(other.graphics_, nullptr);
    aabb_     = std::move(other.aabb_);
  }
  return *this;
}

void ktp::AerolitePhysicsComponent::createB2Body(AerolitePhysicsComponent& aerolite) {
  if (aerolite.body_) world_->DestroyBody(aerolite.body_);

  b2BodyDef body_def {};
  body_def.type = b2_dynamicBody;
  body_def.userData.pointer = reinterpret_cast<uintptr_t>(aerolite.owner_);
  aerolite.body_ = world_->CreateBody(&body_def);

  b2PolygonShape aerolite_shape {};
  // you need the -1 because shape_ is always sides + 1 due to the closing point
  aerolite_shape.Set(aerolite.shape_.data(), aerolite.shape_.size() - 1);

  b2FixtureDef fixture_def {};
  fixture_def.shape = &aerolite_shape;
  fixture_def.density = ConfigParser::aerolites_config.density_;
  fixture_def.friction = ConfigParser::aerolites_config.friction_;
  fixture_def.restitution = ConfigParser::aerolites_config.restitution_;
  aerolite.body_->CreateFixture(&fixture_def);
}

void ktp::AerolitePhysicsComponent::generateAeroliteShape(B2Vec2Vector& shape, float size) {
  generateAeroliteShape(shape, size, generateRand(kMinSides_, kMaxSides_));
}

void ktp::AerolitePhysicsComponent::generateAeroliteShape(B2Vec2Vector& shape, float size, unsigned int sides) {
  if (!shape.empty()) shape.clear();
  b2Vec2 point {};
  for (auto i = 0u; i < sides; ++i) {
    point.x = size * SDL_cosf(2 * b2_pi * i / sides);
    point.y = size * SDL_sinf(2 * b2_pi * i / sides);
    shape.push_back(point);
  }
  // this is the closing point == first point
  shape.push_back(shape.front());
  shape.shrink_to_fit();
}

void ktp::AerolitePhysicsComponent::resize(float size) {
  size_ = size;
  generateAeroliteShape(shape_, size_, shape_.size() - 1u);
  graphics_->renderShape().clear();
  graphics_->renderShape().resize(shape_.size());
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

void ktp::AerolitePhysicsComponent::spawnAerolite() {
  const auto aerolite {GameEntity::createEntity(EntityTypes::Aerolite)};
  if (!aerolite) return;
  static int side {};
  const float delta {ConfigParser::aerolites_config.speed_.value_ * generateRand(ConfigParser::aerolites_config.speed_.rand_min_, ConfigParser::aerolites_config.speed_.rand_max_)};
  switch (side) {
    case 0: // up
      aerolite->physics()->body()->SetTransform({b2_screen_size_.x * 0.5f, 0.f}, aerolite->physics()->body()->GetAngle());
      aerolite->physics()->body()->SetLinearVelocity({0, delta});
      ++side;
      break;
    case 1: // right
      aerolite->physics()->body()->SetTransform({b2_screen_size_.x, b2_screen_size_.y * 0.5f}, aerolite->physics()->body()->GetAngle());
      aerolite->physics()->body()->SetLinearVelocity({-delta, 0});
      ++side;
      break;
    case 2: // down
      aerolite->physics()->body()->SetTransform({b2_screen_size_.x * 0.5f, b2_screen_size_.y}, aerolite->physics()->body()->GetAngle());
      aerolite->physics()->body()->SetLinearVelocity({0, -delta});
      ++side;
      break;
    case 3: // left
      aerolite->physics()->body()->SetTransform({0.f, b2_screen_size_.y * 0.5f}, aerolite->physics()->body()->GetAngle());
      aerolite->physics()->body()->SetLinearVelocity({delta, 0});
      side = 0;
      break;
  }
}

void ktp::AerolitePhysicsComponent::split() {
  collided_ = false;
  if (size_ < kMinSize_) {
    // very small, destroyed on impact
    owner_->deactivate();
    kuge::AeroliteDestroyedEvent ev {
      kuge::KugeEventTypes::AeroliteDestroyed,
      {owner_->physics()->body()->GetPosition().x * kMetersToPixels,
       owner_->physics()->body()->GetPosition().y * kMetersToPixels},
       kScore_ / size_};
    owner_->event_bus_->postEvent(ev);
    return;
  }

  const auto old_angle   {body_->GetAngle()};
  const auto old_angular {body_->GetAngularVelocity()};
  const auto old_delta   {body_->GetLinearVelocity()};
  const auto old_pos     {body_->GetPosition()};
  unsigned int pieces;
  float piece_size;

  if (size_ > 2) {
    // big aerolite, can split in several small aerolites
    const int scale = 10 * generateRand(0.f, 1.f);
    switch (scale) {
      case 0:
      case 1:
      case 2:
      case 3:
      case 4:
      case 5:
      case 6:
        pieces = 1;
        piece_size = size_ * 0.25f;
        resize(size_ * 0.75f);
        break;
      case 7:
      case 8:
      case 9:
        pieces = 2;
        piece_size = size_ * 0.25f;
        resize(size_ * 0.5f);
        break;
      case 10:
        pieces = 3;
        piece_size = size_ * 0.25f;
        resize(size_ * 0.25f);
        break;
    }
  } else if (size_ > 1) {
    // medium aerolite, can split in 1 small aerolite
    pieces = 1;
    piece_size = size_ * 0.33f;
    resize(size_ * 0.66f);
  } else {
    // little aerolite, splits in half
    pieces = 1;
    piece_size = size_ * 0.5f;
    resize(size_ * 0.5f);
  }

  GameEntity* aerolite {nullptr};
  for (std::size_t i = 0; i < pieces; ++i) {
    aerolite = GameEntity::createEntity(EntityTypes::Aerolite);
    if (!aerolite) return;
    static_cast<AerolitePhysicsComponent*>(aerolite->physics())->resize(piece_size); // need this until we can use the size constructor
    aerolite->physics()->body()->SetAngularVelocity(-old_angular * generateRand(0.5f, 1.5f));
    aerolite->physics()->body()->SetTransform(old_pos, old_angle);
    aerolite->physics()->body()->SetLinearVelocity({old_delta.x * generateRand(0.5f, 1.5f), old_delta.y * generateRand(0.5f, 1.5f)});
  }

  kuge::AeroliteSplittedEvent ev {
    kuge::KugeEventTypes::AeroliteSplitted,
    {owner_->physics()->body()->GetPosition().x * kMetersToPixels,
     owner_->physics()->body()->GetPosition().y * kMetersToPixels},
    pieces,
    ((kScore_ / 10u) / size_) * (pieces + 1u)
  };
  owner_->event_bus_->postEvent(ev);
}

void ktp::AerolitePhysicsComponent::transformRenderShape() {
  for (auto i = 0u; i < shape_.size(); ++i) {
    graphics_->renderShape().data()[i].x = ((shape_[i].x * SDL_cosf(body_->GetAngle()) - shape_[i].y * SDL_sinf(body_->GetAngle())) + body_->GetPosition().x) * kMetersToPixels;
    graphics_->renderShape().data()[i].y = ((shape_[i].x * SDL_sinf(body_->GetAngle()) + shape_[i].y * SDL_cosf(body_->GetAngle())) + body_->GetPosition().y) * kMetersToPixels;
  }
}

void ktp::AerolitePhysicsComponent::update(const GameEntity& aerolite, float delta_time) {
  if (collided_) split();

  transformRenderShape();

  aabb_ = body_->GetFixtureList()->GetAABB(0);

  constexpr auto threshold {0.1f};
  if (aabb_.upperBound.x < 0 || aabb_.lowerBound.x > b2_screen_size_.x + threshold
   || aabb_.upperBound.y < 0 || aabb_.lowerBound.y > b2_screen_size_.y + threshold) {
    owner_->deactivate();
  }
  // this will be usefull when the body_ has n fixtures
  /*
  aabb_.lowerBound = b2Vec2( FLT_MAX, FLT_MAX);
  aabb_.upperBound = b2Vec2(-FLT_MAX,-FLT_MAX);
  for (b2Fixture* fixture = body_->GetFixtureList(); fixture; fixture = fixture->GetNext()) {
    aabb_.Combine(aabb_, fixture->GetAABB(0)); /// wtf is childIndex
  } */
}
