#include "include/aerolite.hpp"
#include "include/box2d_utils.hpp"
#include "include/palette.hpp"
#include "include/random.hpp"
#include "include/resources.hpp"
#include "include/testing.hpp"
#include "sdl2_wrappers/sdl2_geometry.hpp"
#include "sdl2_wrappers/sdl2_log.hpp"
#include "sdl2_wrappers/sdl2_timer.hpp"

void ktp::Testing::draw() const {
  shader_program_.use();
  vao_.bind();
  glDrawElements(GL_TRIANGLES, (GLsizei)indices_data_.size(), GL_UNSIGNED_INT, 0);
}

void ktp::Testing::init() {
  shader_program_ = Resources::getShader("testing");

  // vertices_ = {
  //   // first triangle
  //    0.5f,  0.5f, 0.0f,  // top right
  //    0.5f, -0.5f, 0.0f,  // bottom right
  //   -0.5f,  0.5f, 0.0f,  // top left
  //   // second triangle
  //    0.5f, -0.5f, 0.0f,  // bottom right
  //   -0.5f, -0.5f, 0.0f,  // bottom left
  //   -0.5f,  0.5f, 0.0f   // top left
  // };

  vertices_data_ = SDL2_GL::cube(0.5f);
  EBO::generateEBO(vertices_data_, indices_data_);
  vao_.bind();
  indices_.setup(indices_data_);
  vertices_.setup(vertices_data_);
  vao_.linkAttrib(vertices_, 0, 3, GL_FLOAT, 0, nullptr);

  vertices_color_data_.resize(vertices_data_.size());
  for (auto& i: vertices_color_data_) {
    i = generateRand(0.f, 1.f);
  }
  colors_.setup(vertices_color_data_);
  vao_.linkAttrib(colors_, 1, 3, GL_FLOAT, 0, nullptr);
}

void ktp::Testing::update(float delta_time) {
  updateCamera(delta_time);
  updateMVP(delta_time);
}

void ktp::Testing::updateCamera(float delta_time) {
  const auto state {SDL_GetKeyboardState(nullptr)};
  const float kSpeed {1.5f * delta_time};
  if (state[SDL_SCANCODE_W]) {
    camera_pos_ += kSpeed * camera_front_;
  }
  if (state[SDL_SCANCODE_S]) {
    camera_pos_ -= kSpeed * camera_front_;
  }
  if (state[SDL_SCANCODE_A]) {
    camera_pos_ -= glm::normalize(glm::cross(camera_front_, camera_up_)) * kSpeed;
  }
  if (state[SDL_SCANCODE_D]) {
    camera_pos_ += glm::normalize(glm::cross(camera_front_, camera_up_)) * kSpeed;
  }
}

void ktp::Testing::updateMouse(float x_pos, float y_pos) {
  constexpr float sensitivity {0.1f};

  yaw_   += x_pos * sensitivity;
  pitch_ -= y_pos * sensitivity;;

  if (pitch_ >  89.0f) pitch_ =  89.0f;
  if (pitch_ < -89.0f) pitch_ = -89.0f;

  glm::vec3 direction {};
  direction.x = SDL_cosf(glm::radians(yaw_)) * SDL_cosf(glm::radians(pitch_));
  direction.y = SDL_sinf(glm::radians(pitch_));
  direction.z = SDL_sinf(glm::radians(yaw_)) * SDL_cosf(glm::radians(pitch_));
  camera_front_ = glm::normalize(direction);
}

void ktp::Testing::updateMVP(float delta_time) {
  glm::mat4 model {1.f};
  view_ = glm::lookAt(camera_pos_, camera_pos_ + camera_front_, camera_up_);
  mvp_ = projection_ * view_ * model;
  shader_program_.setMat4f("mvp", glm::value_ptr(mvp_));
}
