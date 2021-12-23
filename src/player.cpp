#include "include/emitter.hpp"
#include "include/game_entity.hpp"
#include "include/player.hpp"
#include "include/resources.hpp"

/* GRAPHICS */

ktp::PlayerGraphicsComponent::PlayerGraphicsComponent() noexcept:
  shader_(Resources::getShader("player")) {

  generateOpenGLStuff(ConfigParser::player_config.size_ * kMetersToPixels);
}

void ktp::PlayerGraphicsComponent::generateOpenGLStuff(float size) {
  const GLfloatVector player_shape {
     0.00f * size,  0.50f * size, 0.f,  color_.r,  color_.g,  color_.b,   // top        0
    -0.33f * size, -0.50f * size, 0.f,  color_.r,  color_.g,  color_.b,   // left       1
    -0.15f * size, -0.33f * size, 0.f,  color_.r,  color_.g,  color_.b,   // left flap  2
     0.15f * size, -0.33f * size, 0.f,  color_.r,  color_.g,  color_.b,   // right flap 3
     0.33f * size, -0.50f * size, 0.f,  color_.r,  color_.g,  color_.b    // right      4
  };
  const GLuintVector player_shape_indices {
    0, 1, 2,
    0, 2, 3,
    0, 3, 4
  };
  vertices_.setup(player_shape);
  // vertices
  vao_.linkAttrib(vertices_, 0, 3, GL_FLOAT, 6 * sizeof(GLfloat), nullptr);
  // colors
  vao_.linkAttrib(vertices_, 1, 3, GL_FLOAT, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GL_FLOAT)));
  // EBO
  vertices_indices_.setup(player_shape_indices);
}

void ktp::PlayerGraphicsComponent::update(const GameEntity& player) {
  shader_.setMat4f("mvp", glm::value_ptr(mvp_));
  vao_.bind();
  glDrawElements(GL_TRIANGLES, 9, GL_UNSIGNED_INT, 0); // 9 is the number of indices
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
  vertices[0].Set(           0.f,  size_ * 0.50f); // top
  vertices[1].Set(-size_ * 0.15f, -size_ * 0.33f); // left
  vertices[2].Set( size_ * 0.15f, -size_ * 0.33f); // right
  b2PolygonShape triangle {};
  triangle.Set(vertices, 3);

  b2FixtureDef fixture_def {};
  fixture_def.density = ConfigParser::player_config.density_;
  fixture_def.filter.groupIndex = -1;
  fixture_def.friction = ConfigParser::player_config.friction_;
  fixture_def.shape = &triangle;

  body_->CreateFixture(&fixture_def);

  // left triangle CCW
  vertices[0].Set(           0.f,  size_ * 0.50f); // top
  vertices[1].Set(-size_ * 0.33f, -size_ * 0.50f); // left
  vertices[2].Set(-size_ * 0.15f, -size_ * 0.33f); // right
  triangle.Set(vertices, 3);
  fixture_def.shape = &triangle;

  body_->CreateFixture(&fixture_def);

  // right triangle CCW
  vertices[0].Set(          0.f,  size_ * 0.50f); // top
  vertices[1].Set(size_ * 0.15f, -size_ * 0.33f); // left
  vertices[2].Set(size_ * 0.33f, -size_ * 0.50f); // right
  triangle.Set(vertices, 3);
  fixture_def.shape = &triangle;

  body_->CreateFixture(&fixture_def);
}

void ktp::PlayerPhysicsComponent::update(const GameEntity& player, float delta_time) {
  checkWrap();
  updateMVP();
  // cos_ = SDL_cosf(body_->GetAngle());
  // sin_ = SDL_sinf(body_->GetAngle());
  // exhaust_emitter_->setAngle(body_->GetAngle());
  // exhaust_emitter_->setPosition({
  //   (body_->GetPosition().x * kMetersToPixels) - size_ * 0.33f * kMetersToPixels * sin_,
  //   (body_->GetPosition().y * kMetersToPixels) + size_ * 0.33f * kMetersToPixels * cos_
  // });
  // exhaust_emitter_->update(player, delta_time);
  // if (thrusting_) exhaust_emitter_->generateParticles();
}

void ktp::PlayerPhysicsComponent::updateMVP() {
  glm::mat4 model {1.f};
  model = glm::translate(model, glm::vec3(body_->GetPosition().x * kMetersToPixels, body_->GetPosition().y * kMetersToPixels, 0.f));
  model = glm::rotate(model, body_->GetAngle(), glm::vec3(0.f, 0.f, 1.f));
  graphics_->mvp_ = projection_ * model;
}
