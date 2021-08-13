#include "include/box2d_scale.hpp"
#include "include/emitter.hpp"
#include "include/game_entity.hpp"
#include "include/player.hpp"
#include "../sdl2_wrappers/sdl2_renderer.hpp"
#include <box2d/box2d.h>

/* GRAPHICS */

ktp::PlayerGraphicsComponent::PlayerGraphicsComponent() noexcept {
  exhaust_emitter_ = std::make_unique<EmitterGraphicsComponent>();
}

void ktp::PlayerGraphicsComponent::update(const GameEntity& player, const SDL2_Renderer& renderer) {
  // player's shape
  renderer.setDrawColor(color_);
  renderer.drawLines(render_shape_);
  // player's thrust fx
  if (thrusting_) {
    renderer.setDrawColor(Colors::red);
    renderer.drawLines(render_flame_shape_);
  }
  exhaust_emitter_->update(player, renderer);
}

/* DEMO INPUT */

void ktp::DemoInputComponent::update(GameEntity& player, float delta_time) {
  stopSteering(delta_time);
  shoot(player);

  thrust_ ? thrust(player, delta_time) : stopThrusting(player);

  if (SDL2_Timer::SDL2Ticks() - thrusting_timer_ > kThrustingInterval_) {
    thrust_ = !thrust_;
    thrusting_timer_ = SDL2_Timer::SDL2Ticks();
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

ktp::PlayerPhysicsComponent::PlayerPhysicsComponent(GameEntity* owner,PlayerGraphicsComponent* graphics) noexcept:
 graphics_(graphics) {
  owner_ = owner;
  size_ = ConfigParser::player_config.size_;
  flame_max_lenght_ = size_;
  flame_min_lenght_ = size_ * 0.4f;
  generatePlayerShape(shape_, flame_shape_, size_);
  graphics_->render_shape_.resize(shape_.size());
  graphics_->render_flame_shape_.resize(flame_shape_.size());
  setBox2D();
  exhaust_emitter_ = std::make_unique<EmitterPhysicsComponent>(EmitterPhysicsComponent::makeEmitter(graphics_->exhaust_emitter_.get(), "fire", {body_->GetPosition().x, body_->GetPosition().y}));
}

ktp::PlayerPhysicsComponent& ktp::PlayerPhysicsComponent::operator=(PlayerPhysicsComponent&& other) noexcept {
  if (this != &other) {
    // inherited members
    body_     = other.body_;
    collided_ = other.collided_;
    delta_    = std::move(other.delta_);
    owner_    = std::exchange(other.owner_, nullptr);
    shape_    = std::move(other.shape_);
    size_     = other.size_;
    // own members
    graphics_            = std::exchange(other.graphics_, nullptr);
    flame_shape_         = std::move(other.flame_shape_);
    thrusting_           = other.thrusting_;
    cos_                 = other.cos_;
    sin_                 = other.sin_;
    flame_growth_factor_ = other.flame_growth_factor_;
    flame_max_lenght_    = other.flame_max_lenght_;
    flame_min_lenght_    = other.flame_min_lenght_;
    exhaust_emitter_     = std::move(other.exhaust_emitter_);
  }
  return *this;
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

void ktp::PlayerPhysicsComponent::generatePlayerShape(B2Vec2Vector& shape, FPointsVector& flame_shape, float size) {
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
  body_def.userData.pointer = reinterpret_cast<uintptr_t>(owner_);

  body_ = world_->CreateBody(&body_def);
  // middle triangle CCW
  b2Vec2 vertices[3];
  vertices[0].Set(           0.f, -size_ * 0.50f); // top
  vertices[1].Set(-size_ * 0.15f,  size_ * 0.33f); // left
  vertices[2].Set( size_ * 0.15f,  size_ * 0.33f); // right
  b2PolygonShape triangle {};
  triangle.Set(vertices, 3);

  b2FixtureDef fixture_def {};
  fixture_def.density = ConfigParser::player_config.density_;
  fixture_def.filter.groupIndex = -1;
  fixture_def.friction = ConfigParser::player_config.friction_;
  fixture_def.shape = &triangle;

  body_->CreateFixture(&fixture_def);

  // left triangle CCW
  vertices[0].Set(           0.f, -size_ * 0.50f); // top
  vertices[1].Set(-size_ * 0.33f,  size_ * 0.50f); // left
  vertices[2].Set(-size_ * 0.15f,  size_ * 0.33f); // right
  triangle.Set(vertices, 3);
  fixture_def.shape = &triangle;

  body_->CreateFixture(&fixture_def);

  // right triangle CCW
  vertices[0].Set(          0.f, -size_ * 0.50f); // top
  vertices[1].Set(size_ * 0.15f,  size_ * 0.33f); // left
  vertices[2].Set(size_ * 0.33f,  size_ * 0.50f); // right
  triangle.Set(vertices, 3);
  fixture_def.shape = &triangle;

  body_->CreateFixture(&fixture_def);
}

void ktp::PlayerPhysicsComponent::transformRenderShape() {
  for (auto i = 0u; i < shape_.size(); ++i) {
    graphics_->render_shape_[i].x = ((shape_[i].x * cos_ - shape_[i].y * sin_) + body_->GetPosition().x) * kMetersToPixels;
    graphics_->render_shape_[i].y = ((shape_[i].x * sin_ + shape_[i].y * cos_) + body_->GetPosition().y) * kMetersToPixels;
  }
  if (thrusting_) {
    for (auto i = 0u; i < flame_shape_.size(); ++i) {
      graphics_->render_flame_shape_[i].x = ((flame_shape_[i].x * cos_ - flame_shape_[i].y * sin_) + body_->GetPosition().x) * kMetersToPixels;
      graphics_->render_flame_shape_[i].y = ((flame_shape_[i].x * sin_ + flame_shape_[i].y * cos_) + body_->GetPosition().y) * kMetersToPixels;
    }
  }
}

void ktp::PlayerPhysicsComponent::update(const GameEntity& player, float delta_time) {
  graphics_->thrusting_ = thrusting_;
  checkWrap();
  cos_ = SDL_cosf(body_->GetAngle());
  sin_ = SDL_sinf(body_->GetAngle());
  transformRenderShape();
  exhaust_emitter_->setPosition({
    (body_->GetPosition().x * kMetersToPixels) - size_ * 0.33f * kMetersToPixels * sin_,
    (body_->GetPosition().y * kMetersToPixels) + size_ * 0.33f * kMetersToPixels * cos_
  });
  exhaust_emitter_->update(player, delta_time);
  if (thrusting_) exhaust_emitter_->generateParticles();
}
