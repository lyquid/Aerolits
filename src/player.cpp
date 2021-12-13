#include "include/box2d_utils.hpp"
#include "include/emitter.hpp"
#include "include/game_entity.hpp"
#include "include/paths.hpp"
#include "include/player.hpp"
#include "sdl2_wrappers/sdl2_renderer.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <box2d/box2d.h>

/* GRAPHICS */

ktp::PlayerGraphicsComponent::PlayerGraphicsComponent() noexcept {
  SDL2ColorToB2Color(ConfigParser::player_config.color_, color_);
  const GLfloatVector color_data {
    color_.r,  color_.g,  color_.b,
    color_.r,  color_.g,  color_.b,
    color_.r,  color_.g,  color_.b,
    color_.r,  color_.g,  color_.b,
    color_.r,  color_.g,  color_.b,
    color_.r,  color_.g,  color_.b,
    color_.r,  color_.g,  color_.b,
    color_.r,  color_.g,  color_.b,
    color_.r,  color_.g,  color_.b
  };
  const auto shape {generatePlayerRenderShape(ConfigParser::player_config.size_ * kMetersToPixels)};
  projection_ = glm::ortho(
    0.f, PhysicsComponent::b2ScreenSize().x * kMetersToPixels, // left, right
    0.f, PhysicsComponent::b2ScreenSize().y * kMetersToPixels ,// bottom, top
    -1.f, 1.f // zNear, zFar
  ); // this should be somewhere else

  vertices_.setup(shape);
  vao_.linkAttrib(vertices_, 0, 3, GL_FLOAT, 0, nullptr);
  colors_.setup(color_data);
  vao_.linkAttrib(colors_, 1, 3, GL_FLOAT, 0, nullptr);

  const auto vertex_shader_path {getResourcesPath("shaders") + "default.vert"};
  const auto fragment_shader_path {getResourcesPath("shaders") + "default.frag"};
  shader_.setup(vertex_shader_path, fragment_shader_path);
  shader_.setMat4f("projection", glm::value_ptr(projection_));
}

ktp::GLfloatVector ktp::PlayerGraphicsComponent::generatePlayerRenderShape(float size) {
  return GLfloatVector {
     0.00f * size,  0.50f * size, 0.f * size, // top
    -0.33f * size, -0.50f * size, 0.f * size, // left
    -0.15f * size, -0.33f * size, 0.f * size, // left flap

     0.00f * size,  0.50f * size, 0.f * size, // top
    -0.15f * size, -0.33f * size, 0.f * size, // left flap
     0.15f * size, -0.33f * size, 0.f * size, // right flap

     0.00f * size,  0.50f * size, 0.f * size, // top
     0.15f * size, -0.33f * size, 0.f * size, // right flap
     0.33f * size, -0.50f * size, 0.f * size  // right
  };
}

void ktp::PlayerGraphicsComponent::update(const GameEntity& player) {
  shader_.use();
  vao_.bind();
  glDrawArrays(GL_TRIANGLES, 0, 9);
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
    steer(angular_impulse_);
  }

  if (state[SDL_SCANCODE_D] || state[SDL_SCANCODE_RIGHT]) {
    // steer right
    steer(-angular_impulse_);
  }

  if (state[SDL_SCANCODE_SPACE]) {
    shoot(player);
  }
}

/* PHYSICS */

ktp::PlayerPhysicsComponent::PlayerPhysicsComponent(GameEntity* owner, PlayerGraphicsComponent* graphics) noexcept:
 graphics_(graphics) {
  owner_ = owner;
  size_ = ConfigParser::player_config.size_;
  setBox2D();
  // exhaust_emitter_ = std::make_unique<EmitterPhysicsComponent>(EmitterPhysicsComponent::makeEmitter(graphics_->exhaust_emitter_.get(), "fire", {body_->GetPosition().x, body_->GetPosition().y}));
}

ktp::PlayerPhysicsComponent& ktp::PlayerPhysicsComponent::operator=(PlayerPhysicsComponent&& other) noexcept {
  if (this != &other) {
    // inherited members
    body_     = other.body_;
    collided_ = other.collided_;
    delta_    = std::move(other.delta_);
    owner_    = std::exchange(other.owner_, nullptr);
    size_     = other.size_;
    // own members
    graphics_        = std::exchange(other.graphics_, nullptr);
    thrusting_       = other.thrusting_;
    cos_             = other.cos_;
    sin_             = other.sin_;
    // exhaust_emitter_ = std::move(other.exhaust_emitter_);
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
  glm::mat4 model {1.f};
  model = glm::translate(model, glm::vec3(body_->GetPosition().x * kMetersToPixels, body_->GetPosition().y * kMetersToPixels, 0.f));
  model = glm::rotate(model, body_->GetAngle(), glm::vec3(0.f, 0.f, 1.f));
  graphics_->shader_.setMat4f("model", glm::value_ptr(model));
}

void ktp::PlayerPhysicsComponent::update(const GameEntity& player, float delta_time) {
  checkWrap();
  // cos_ = SDL_cosf(body_->GetAngle());
  // sin_ = SDL_sinf(body_->GetAngle());
  transformRenderShape();
  // exhaust_emitter_->setAngle(body_->GetAngle());
  // exhaust_emitter_->setPosition({
  //   (body_->GetPosition().x * kMetersToPixels) - size_ * 0.33f * kMetersToPixels * sin_,
  //   (body_->GetPosition().y * kMetersToPixels) + size_ * 0.33f * kMetersToPixels * cos_
  // });
  // exhaust_emitter_->update(player, delta_time);
  // if (thrusting_) exhaust_emitter_->generateParticles();
}
