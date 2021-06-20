#include "include/physics_component.hpp"
#include "include/game_entity.hpp"
#include "include/graphics_component.hpp"
#include "include/input_component.hpp"
#include "include/box2d_scale.hpp"
#include "include/random.hpp"

SDL_FPoint ktp::PhysicsComponent::b2_screen_size_ {};
b2World* ktp::PhysicsComponent::world_ {nullptr};

void ktp::PhysicsComponent::setScreenSize(const SDL_FPoint& screen_size) {
  b2_screen_size_.x = screen_size.x / kMetersToPixels;
  b2_screen_size_.y = screen_size.y / kMetersToPixels;
}

/* PlayerPhysicsComponent */

ktp::PlayerPhysicsComponent::PlayerPhysicsComponent(PlayerGraphicsComponent* graphics): 
 graphics_(graphics) {
  size_ = kDefaultPlayerSize_;
  generatePlayerShape(shape_, flame_shape_, size_);
  graphics_->render_shape_.resize(shape_.size());
  graphics_->render_flame_shape_.resize(flame_shape_.size());
  setBox2D();
}

void ktp::PlayerPhysicsComponent::checkWrap() {
  const auto threshold {size_ * 0.5f};
  if (body_->GetPosition().x < -threshold) {
    body_->SetTransform({b2_screen_size_.x + threshold, body_->GetPosition().y}, body_->GetAngle());
  } else if (body_->GetPosition().x > b2_screen_size_.x + threshold) {
    body_->SetTransform({-threshold, body_->GetPosition().y}, body_->GetAngle());
  }
  if (body_->GetPosition().y < -threshold) {
    body_->SetTransform({body_->GetPosition().x, b2_screen_size_.y + threshold}, body_->GetAngle());
  } else if (body_->GetPosition().y > b2_screen_size_.y + threshold) {
    body_->SetTransform({body_->GetPosition().x, -threshold}, body_->GetAngle());
  }
}

void ktp::PlayerPhysicsComponent::generatePlayerShape(FPointsVector& shape, FPointsVector& flame_shape, float size) {
  shape.push_back({          0.f, -size * 0.50f}); // top
  shape.push_back({-size * 0.33f,  size * 0.50f}); // left
  shape.push_back({-size * 0.15f,  size * 0.33f}); // left flap
  shape.push_back({ size * 0.15f,  size * 0.33f}); // right flap
  shape.push_back({ size * 0.33f,  size * 0.50f}); // right
  shape.push_back({          0.f, -size * 0.50f}); // top again
  shape.shrink_to_fit();
  
  flame_shape.push_back({          0.f, size * 0.40f}); // bottom           2 ____ 1
  flame_shape.push_back({ size * 0.15f, size * 0.35f}); // right vertice      \  /
  flame_shape.push_back({-size * 0.15f, size * 0.35f}); // left vertice        \/
  flame_shape.push_back({          0.f, size * 0.40f}); // bottom again        3/0
  flame_shape.shrink_to_fit();
}

void ktp::PlayerPhysicsComponent::setBox2D() {
  // Player
  b2BodyDef body_def {};
  body_def.type = b2_dynamicBody;
  body_def.bullet = true;
  body_def.position.Set(b2_screen_size_.x * 0.5f, b2_screen_size_.y * 0.5f);
  body_def.userData.pointer = reinterpret_cast<uintptr_t>(this);

  body_ = world_->CreateBody(&body_def);
  // triangle CCW
  b2Vec2 vertices[3];
  vertices[0].Set(           0.f, -size_ * 0.50f); // top
  vertices[1].Set(-size_ * 0.33f,  size_ * 0.50f); // left
  vertices[2].Set( size_ * 0.33f,  size_ * 0.50f); // right
  b2PolygonShape triangle {};
  triangle.Set(vertices, 3);

  b2FixtureDef fixture_def {};
  fixture_def.shape = &triangle;
  fixture_def.density = 1.5f;
  fixture_def.friction = 0.8f;
  // https://www.iforce2d.net/b2dtut/collision-filtering
  fixture_def.filter.groupIndex = -1;

  body_->CreateFixture(&fixture_def);

  // Lasers
  /* laser_body_def_.type = b2_dynamicBody;
  laser_body_def_.bullet = true;
  
  b2Vec2 laser_vertices[4];
  laser_vertices[0].Set(-kDefaultLaserSize_ * 0.15f, -kDefaultLaserSize_); // top left
  laser_vertices[1].Set(-kDefaultLaserSize_ * 0.15f,  kDefaultLaserSize_); // down left
  laser_vertices[2].Set( kDefaultLaserSize_ * 0.15f,  kDefaultLaserSize_); // down right
  laser_vertices[3].Set( kDefaultLaserSize_ * 0.15f, -kDefaultLaserSize_); // up right
  laser_shape_b2_.Set(laser_vertices, 4);

  laser_fixture_def_.shape = &laser_shape_b2_;
  laser_fixture_def_.density = 10.f;
  laser_fixture_def_.friction = 0.1f;
  laser_fixture_def_.filter.groupIndex = -1;
  laser_fixture_def_.restitution = 0.35f; */
}

void ktp::PlayerPhysicsComponent::transformRenderShape() {
  for (auto i = 0u; i < shape_.size(); ++i) {
    graphics_->render_shape_[i].x = ((shape_[i].x * SDL_cosf(body_->GetAngle()) - shape_[i].y * SDL_sinf(body_->GetAngle())) + body_->GetPosition().x) * kMetersToPixels;
    graphics_->render_shape_[i].y = ((shape_[i].x * SDL_sinf(body_->GetAngle()) + shape_[i].y * SDL_cosf(body_->GetAngle())) + body_->GetPosition().y) * kMetersToPixels;
  }
  if (thrusting_) {
    for (auto i = 0u; i < flame_shape_.size(); ++i) {
      graphics_->render_flame_shape_[i].x = ((flame_shape_[i].x * SDL_cosf(body_->GetAngle()) - flame_shape_[i].y * SDL_sinf(body_->GetAngle())) + body_->GetPosition().x) * kMetersToPixels;
      graphics_->render_flame_shape_[i].y = ((flame_shape_[i].x * SDL_sinf(body_->GetAngle()) + flame_shape_[i].y * SDL_cosf(body_->GetAngle())) + body_->GetPosition().y) * kMetersToPixels;
    }
  }
}

void ktp::PlayerPhysicsComponent::update(const GameEntity& player, float delta_time) {
  graphics_->thrusting_ = thrusting_;
  checkWrap();
  transformRenderShape();
  
  //exhaust_emitter_.setPosition({body_->GetPosition().x * kMetersToPixels, body_->GetPosition().y * kMetersToPixels});
  //exhaust_emitter_.update();
  //if (!lasers_.empty()) updateLasers();
}

/* AEROLITES */

ktp::AerolitePhysicsComponent::AerolitePhysicsComponent(AeroliteGraphicsComponent* graphics): 
 graphics_(graphics) {
  size_ = kMaxSize_ * generateRand(0.3f, 1.f);
  generateAeroliteShape(shape_, size_);
  graphics_->render_shape_.resize(shape_.size());

  b2BodyDef body_def {};
  body_def.type = b2_dynamicBody;
  
  body_def.userData.pointer = reinterpret_cast<uintptr_t>(this);

  body_ = world_->CreateBody(&body_def);

  b2PolygonShape dynamic_box {};
  dynamic_box.SetAsBox(size_ * 0.5f, size_ * 0.5f);

  b2FixtureDef fixture_def {};
  fixture_def.shape = &dynamic_box;
  fixture_def.density = kKgPerMeter2_ / (size_ * size_);
  fixture_def.friction = 1.f;

  body_->CreateFixture(&fixture_def);

  body_->SetAngularVelocity(kMaxRotationSpeed_ * generateRand(-1.f, 1.f));
}

void ktp::AerolitePhysicsComponent::generateAeroliteShape(FPointsVector& shape, float size) {
  shape.push_back({-size * 0.5f, -size * 0.5f});
  shape.push_back({ size * 0.5f, -size * 0.5f});
  shape.push_back({ size * 0.5f,  size * 0.5f});
  shape.push_back({-size * 0.5f,  size * 0.5f});
  shape.push_back({-size * 0.5f, -size * 0.5f});
  shape.shrink_to_fit();
}

ktp::GameEntity ktp::AerolitePhysicsComponent::spawnAerolite() {
  GameEntity aerolite {GameEntities::Aerolite};
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
  return aerolite;
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