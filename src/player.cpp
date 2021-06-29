#include "include/box2d_scale.hpp"
#include "include/game_entity.hpp"
#include "include/player.hpp"
#include "../sdl2_wrappers/sdl2_renderer.hpp"
#include <box2d/box2d.h>

/* GRAPHICS */

void ktp::PlayerGraphicsComponent::update(const GameEntity& player, const SDL2_Renderer& renderer) {
  // player's shape
  renderer.setDrawColor(kDefaultColor_);
  renderer.drawLines(render_shape_);
  // player's thrust fx
  if (thrusting_) {
    renderer.setDrawColor(Colors::red);
    renderer.drawLines(render_flame_shape_);
  }
  // particles
  // player.exhaust_emitter_.draw();
}

/* DEMO INPUT */

void ktp::DemoInputComponent::update(GameEntity& player, float delta_time) {
  stopSteering(delta_time);
  shoot(player);

  thrust_ ? thrust(player, delta_time) : stopThrusting(player);

  if (SDL2_Timer::getSDL2Ticks() - thrusting_timer_ > kThrustingInterval_) {
    thrust_ = !thrust_;
    thrusting_timer_ = SDL2_Timer::getSDL2Ticks();
  }
}

/* INPUT */

void ktp::PlayerInputComponent::update(GameEntity& player, float delta_time) {
  stopSteering(delta_time);

  const auto state {SDL_GetKeyboardState(nullptr)};

  if (state[SDL_SCANCODE_W] || state[SDL_SCANCODE_UP]) {
    thrust(player, delta_time);
  } else {
    stopThrusting(player);
  }

  if (state[SDL_SCANCODE_A] || state[SDL_SCANCODE_LEFT]) {
    // steer left
    steer(-angular_impulse_);
  }

  if (state[SDL_SCANCODE_D] || state[SDL_SCANCODE_RIGHT]) {
    // steer right
    steer(angular_impulse_);
  }

  if (state[SDL_SCANCODE_SPACE]) {
    shoot(player);
  }
}

/* PHYSICS */

ktp::PlayerPhysicsComponent::PlayerPhysicsComponent(PlayerGraphicsComponent* graphics):
 graphics_(graphics) {
  size_ = kDefaultPlayerSize_;
  generatePlayerShape(shape_, flame_shape_, size_);
  graphics_->render_shape_.resize(shape_.size());
  graphics_->render_flame_shape_.resize(flame_shape_.size());
  setBox2D();
  // flame_emitter_ = std::make_unique<GameEntity>(EmitterPhysicsComponent::makeEmitter("fire", {body_->GetPosition().x, body_->GetPosition().y}));
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

  body_->CreateFixture(&fixture_def);
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
  // flame_emitter_->physics_->setPosition({body_->GetPosition().x * kMetersToPixels, body_->GetPosition().y * kMetersToPixels});
  // flame_emitter_->update(delta_time);
}
