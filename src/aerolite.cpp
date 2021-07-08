#include "include/aerolite.hpp"
#include "include/box2d_scale.hpp"
#include "include/game_entity.hpp"
#include "include/random.hpp"
#include "../sdl2_wrappers/sdl2_renderer.hpp"

/* GRAPHICS */

void ktp::AeroliteGraphicsComponent::update(const GameEntity& aerolite, const SDL2_Renderer& renderer) {
  renderer.setDrawColor(kDefaultColor_);
  renderer.drawLines(render_shape_);
}

/* PHYSICS */

ktp::AerolitePhysicsComponent::AerolitePhysicsComponent(AeroliteGraphicsComponent* graphics) noexcept:
 graphics_(graphics) {
  size_ = kMaxSize_ * generateRand(0.3f, 1.f);
  generateAeroliteShape(shape_, size_);
  graphics_->render_shape_.resize(shape_.size());

  b2BodyDef body_def {};
  body_def.type = b2_dynamicBody;

  body_def.userData.pointer = reinterpret_cast<uintptr_t>(this);

  body_ = world_->CreateBody(&body_def);

  b2PolygonShape dynamic_box {};
  // you need the -1 because shape_ is always sides + 1 due to the closing point
  dynamic_box.Set(shape_.data(), shape_.size() - 1);

  b2FixtureDef fixture_def {};
  fixture_def.shape = &dynamic_box;
  fixture_def.density = kKgPerMeter2_ / (size_ * size_);
  fixture_def.friction = 1.f;

  body_->CreateFixture(&fixture_def);

  body_->SetAngularVelocity(kMaxRotationSpeed_ * generateRand(-1.f, 1.f));
}

ktp::AerolitePhysicsComponent& ktp::AerolitePhysicsComponent::operator=(AerolitePhysicsComponent&& other) noexcept {
  if (this != &other) {
    // inherited members
    body_          = other.body_;
    shape_         = std::move(other.shape_);
    size_          = other.size_;
    to_be_deleted_ = other.to_be_deleted_;
    // own members
    graphics_ = other.graphics_;
    aabb_     = std::move(other.aabb_);
    // tidy
    other.body_     = nullptr;
    other.graphics_ = nullptr;
  }
  return *this;
}

void ktp::AerolitePhysicsComponent::generateAeroliteShape(B2Vec2Vector& shape, float size) {
  const auto sides {generateRand(kMinSides_, kMaxSides_)};
  constexpr auto kPI {3.14159265358979323846264338327950288};
  b2Vec2 point {};
  for (auto i = 0u; i < sides; ++i) {
    point.x = size * SDL_cosf(2 * kPI * i / sides);
    point.y = size * SDL_sinf(2 * kPI * i / sides);
    shape.push_back(point);
  }
  // this is the closing point == first point
  shape.push_back(shape.front());
  shape.shrink_to_fit();
}

ktp::GameEntity ktp::AerolitePhysicsComponent::spawnAerolite() {
  GameEntity aerolite {GameEntity::createEntity(GameEntities::Aerolite)};
  static int side {};
  const float delta {kMaxSpeed_ * generateRand(0.1f, 1.f)};
  switch (side) {
    case 0: // up
      aerolite.physics_->getBody()->SetTransform({b2_screen_size_.x * 0.5f, 0.f}, aerolite.physics_->getBody()->GetAngle());
      aerolite.physics_->getBody()->SetLinearVelocity({0, delta});
      ++side;
      break;
    case 1: // right
      aerolite.physics_->getBody()->SetTransform({b2_screen_size_.x, b2_screen_size_.y * 0.5f}, aerolite.physics_->getBody()->GetAngle());
      aerolite.physics_->getBody()->SetLinearVelocity({-delta, 0});
      ++side;
      break;
    case 2: // down
      aerolite.physics_->getBody()->SetTransform({b2_screen_size_.x * 0.5f, b2_screen_size_.y}, aerolite.physics_->getBody()->GetAngle());
      aerolite.physics_->getBody()->SetLinearVelocity({0, -delta});
      ++side;
      break;
    case 3: // left
      aerolite.physics_->getBody()->SetTransform({0.f, b2_screen_size_.y * 0.5f}, aerolite.physics_->getBody()->GetAngle());
      aerolite.physics_->getBody()->SetLinearVelocity({delta, 0});
      side = 0;
      break;
  }
  return std::move(aerolite);
}

void ktp::AerolitePhysicsComponent::transformRenderShape() {
  for (auto i = 0u; i < shape_.size(); ++i) {
    graphics_->render_shape_[i].x = ((shape_[i].x * SDL_cosf(body_->GetAngle()) - shape_[i].y * SDL_sinf(body_->GetAngle())) + body_->GetPosition().x) * kMetersToPixels;
    graphics_->render_shape_[i].y = ((shape_[i].x * SDL_sinf(body_->GetAngle()) + shape_[i].y * SDL_cosf(body_->GetAngle())) + body_->GetPosition().y) * kMetersToPixels;
  }
}

void ktp::AerolitePhysicsComponent::update(const GameEntity& aerolite, float delta_time) {
  transformRenderShape();

  aabb_ = body_->GetFixtureList()->GetAABB(0);

  constexpr auto threshold {0.1f};
  if (aabb_.upperBound.x < 0 || aabb_.lowerBound.x > b2_screen_size_.x + threshold
   || aabb_.upperBound.y < 0 || aabb_.lowerBound.y > b2_screen_size_.y + threshold) {
    to_be_deleted_ = true;
  }
  // this will be usefull when the body_ has n fixtures
  /*
  aabb_.lowerBound = b2Vec2( FLT_MAX, FLT_MAX);
  aabb_.upperBound = b2Vec2(-FLT_MAX,-FLT_MAX);
  for (b2Fixture* fixture = body_->GetFixtureList(); fixture; fixture = fixture->GetNext()) {
    aabb_.Combine(aabb_, fixture->GetAABB(0)); /// wtf is childIndex
  } */
}
