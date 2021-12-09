#include "include/box2d_utils.hpp"
#include "include/emitter.hpp"
#include "include/game_entity.hpp"
#include "include/paths.hpp"
#include "include/player.hpp"
#include "sdl2_wrappers/sdl2_geometry.hpp" // to be deleted
#include "sdl2_wrappers/sdl2_renderer.hpp"
#include <box2d/box2d.h>

/* GRAPHICS */

ktp::PlayerGraphicsComponent::PlayerGraphicsComponent() noexcept {
  exhaust_emitter_ = std::make_unique<EmitterGraphicsComponent>();
}

void ktp::PlayerGraphicsComponent::update(const GameEntity& player, const SDL2_Renderer& renderer) {
  renderer.setDrawColor(color_);
  renderer.drawLines(render_shape_);
  exhaust_emitter_->update(player, renderer);
}

/* GL GRAPHICS */

ktp::PlayerGLGraphicsComponent::PlayerGLGraphicsComponent() noexcept {
  SDL2ColorToB2Color(ConfigParser::player_config.color_, color_);
  // FPointsVector shape {generatePlayerRenderShape()};
  auto shape {SDL2_GL::cube(0.5f)};

  glGenVertexArrays(1, &vao_id_);
  glGenBuffers(1, &vbo_id_);
  glGenBuffers(1, &colors_id_);

  glBindVertexArray(vao_id_);
  glCheckError();

  glBindBuffer(GL_ARRAY_BUFFER, vbo_id_);
  glCheckError();
  SDL2_GL::glBufferDataFromVector(GL_ARRAY_BUFFER, shape, GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(
    0,    // index: specifies the index of the generic vertex attribute to be modified. Must match the layout in the shader
    3,                                  // size: specifies the number of components per generic vertex attribute. Must be 1, 2, 3, 4.
    GL_FLOAT,                           // type of the data
    GL_FALSE,                           // normalized: specifies whether fixed-point data values should be normalized
    0,   // stride: specifies the byte offset between consecutive generic vertex attributes
    (void*)0    // pointer: specifies a offset of the first component of the first generic vertex attribute in the array in the data store
  );
  glCheckError();

  const std::vector<GLfloat> g_color_buffer_data = {
    0.583f,  0.771f,  0.014f,
    0.609f,  0.115f,  0.436f,
    0.327f,  0.483f,  0.844f,
    0.822f,  0.569f,  0.201f,
    0.435f,  0.602f,  0.223f,
    0.310f,  0.747f,  0.185f,
    0.597f,  0.770f,  0.761f,
    0.559f,  0.436f,  0.730f,
    0.359f,  0.583f,  0.152f,
    0.483f,  0.596f,  0.789f,
    0.559f,  0.861f,  0.639f,
    0.195f,  0.548f,  0.859f,
    0.014f,  0.184f,  0.576f,
    0.771f,  0.328f,  0.970f,
    0.406f,  0.615f,  0.116f,
    0.676f,  0.977f,  0.133f,
    0.971f,  0.572f,  0.833f,
    0.140f,  0.616f,  0.489f,
    0.997f,  0.513f,  0.064f,
    0.945f,  0.719f,  0.592f,
    0.543f,  0.021f,  0.978f,
    0.279f,  0.317f,  0.505f,
    0.167f,  0.620f,  0.077f,
    0.347f,  0.857f,  0.137f,
    0.055f,  0.953f,  0.042f,
    0.714f,  0.505f,  0.345f,
    0.783f,  0.290f,  0.734f,
    0.722f,  0.645f,  0.174f,
    0.302f,  0.455f,  0.848f,
    0.225f,  0.587f,  0.040f,
    0.517f,  0.713f,  0.338f,
    0.053f,  0.959f,  0.120f,
    0.393f,  0.621f,  0.362f,
    0.673f,  0.211f,  0.457f,
    0.820f,  0.883f,  0.371f,
    0.982f,  0.099f,  0.879f
  };

  glBindBuffer(GL_ARRAY_BUFFER, colors_id_);
  glCheckError();
  SDL2_GL::glBufferDataFromVector(GL_ARRAY_BUFFER, g_color_buffer_data, GL_STATIC_DRAW);

  glEnableVertexAttribArray(1);
  glVertexAttribPointer(
    1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
    3,                                // size
    GL_FLOAT,                         // type
    GL_FALSE,                         // normalized
    0,                                // stride
    (void*)0                          // array buffer offset
  );

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  const auto vertex_shader_path {getResourcesPath("shaders") + "vertex_shader.glsl"};
  const auto fragment_shader_path {getResourcesPath("shaders") + "fragment_shader.glsl"};
  shader_.setup(vertex_shader_path, fragment_shader_path);
}

ktp::FPointsVector ktp::PlayerGLGraphicsComponent::generatePlayerRenderShape() {
  FPointsVector shape {};
  float size {ConfigParser::player_config.size_};
  // shape.push_back({          0.f * kMetersToPixels, -size * 0.50f * kMetersToPixels}); // top
  // shape.push_back({-size * 0.33f * kMetersToPixels,  size * 0.50f * kMetersToPixels}); // left
  // shape.push_back({-size * 0.15f * kMetersToPixels,  size * 0.33f * kMetersToPixels}); // left flap
  // shape.push_back({ size * 0.15f * kMetersToPixels,  size * 0.33f * kMetersToPixels}); // right flap
  // shape.push_back({ size * 0.33f * kMetersToPixels,  size * 0.50f * kMetersToPixels}); // right
  // shape.push_back({          0.f * kMetersToPixels, -size * 0.50f * kMetersToPixels}); // top again
  shape.push_back({          0.f, -size * 0.50f}); // top
  shape.push_back({-size * 0.33f,  size * 0.50f}); // left
  shape.push_back({-size * 0.15f,  size * 0.33f}); // left flap
  shape.push_back({ size * 0.15f,  size * 0.33f}); // right flap
  shape.push_back({ size * 0.33f,  size * 0.50f}); // right
  shape.push_back({          0.f, -size * 0.50f}); // top again
  shape.shrink_to_fit();
  return shape;
}

void ktp::PlayerGLGraphicsComponent::update(const GameEntity& player) {
  shader_.use();
  glBindVertexArray(vao_id_);
  glDrawArrays(GL_TRIANGLES, 0, 36);
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

ktp::PlayerPhysicsComponent::PlayerPhysicsComponent(GameEntity* owner, PlayerGraphicsComponent* graphics) noexcept:
 graphics_(graphics) {
  owner_ = owner;
  size_ = ConfigParser::player_config.size_;
  generatePlayerShape(shape_, size_);
  graphics_->render_shape_.resize(shape_.size());
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
    graphics_        = std::exchange(other.graphics_, nullptr);
    thrusting_       = other.thrusting_;
    cos_             = other.cos_;
    sin_             = other.sin_;
    exhaust_emitter_ = std::move(other.exhaust_emitter_);
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

void ktp::PlayerPhysicsComponent::generatePlayerShape(B2Vec2Vector& shape, float size) {
  shape.push_back({          0.f, -size * 0.50f}); // top
  shape.push_back({-size * 0.33f,  size * 0.50f}); // left
  shape.push_back({-size * 0.15f,  size * 0.33f}); // left flap
  shape.push_back({ size * 0.15f,  size * 0.33f}); // right flap
  shape.push_back({ size * 0.33f,  size * 0.50f}); // right
  shape.push_back({          0.f, -size * 0.50f}); // top again
  shape.shrink_to_fit();
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
}

void ktp::PlayerPhysicsComponent::update(const GameEntity& player, float delta_time) {
  checkWrap();
  cos_ = SDL_cosf(body_->GetAngle());
  sin_ = SDL_sinf(body_->GetAngle());
  transformRenderShape();
  exhaust_emitter_->setAngle(body_->GetAngle());
  exhaust_emitter_->setPosition({
    (body_->GetPosition().x * kMetersToPixels) - size_ * 0.33f * kMetersToPixels * sin_,
    (body_->GetPosition().y * kMetersToPixels) + size_ * 0.33f * kMetersToPixels * cos_
  });
  exhaust_emitter_->update(player, delta_time);
  if (thrusting_) exhaust_emitter_->generateParticles();
}
