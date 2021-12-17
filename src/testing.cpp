#include "include/box2d_utils.hpp"
#include "include/palette.hpp"
#include "include/random.hpp"
#include "include/resources.hpp"
#include "include/testing.hpp"
#include "sdl2_wrappers/sdl2_geometry.hpp"
#include "sdl2_wrappers/sdl2_log.hpp"
#include "sdl2_wrappers/sdl2_timer.hpp"

ktp::Testing::Testing() {}

void ktp::Testing::draw() const {
  shader_program_.use();
  //cube_.draw();
}

void ktp::Testing::tutorial() {
  color_vertices_.resize(108);
  color_vertices_direction_.resize(color_vertices_.size());
  for (auto i = 0; i < color_vertices_.size() / 3; ++i) {
    const auto color {SDL2ColorToB2Color(Colors::palette.at(generateRand(0u, Colors::palette.size() - 1)))};
    color_vertices_[3 * i + 0] = color.r;
    color_vertices_[3 * i + 1] = color.g;
    color_vertices_[3 * i + 2] = color.b;
    color_vertices_direction_[3 * i + 0] = generateRand(0u, 1u);
    color_vertices_direction_[3 * i + 1] = generateRand(0u, 1u);
    color_vertices_direction_[3 * i + 2] = generateRand(0u, 1u);
  }

  // cube_config_.colors_ = color_vertices_;
  // cube_config_.usage_ = GL_STATIC_DRAW;
  // cube_config_.vertices_ = SDL2_GL::cube(0.5f);
  // cube_config_.stride_ = 0;
  //cube_.setup(cube_config_);

  const auto vertex_shader_path {Resources::getResourcesPath("shaders") + "default.vert"};
  const auto fragment_shader_path {Resources::getResourcesPath("shaders") + "default.frag"};
  // shader_program_.setup(vertex_shader_path, fragment_shader_path);

  shader_program_.setMat4f("model", glm::value_ptr(model_));
  shader_program_.setMat4f("view", glm::value_ptr(view_));
  shader_program_.setMat4f("projection", glm::value_ptr(projection_));
}

void ktp::Testing::update(float delta_time) {
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

  auto yaw {-90.f};
  auto pitch {0.f};
  glm::vec3 direction {};
  direction.x = SDL_cosf(glm::radians(yaw)) * SDL_cosf(glm::radians(pitch));
  direction.y = SDL_sinf(glm::radians(pitch));
  direction.z = SDL_sinf(glm::radians(yaw)) * SDL_cosf(glm::radians(pitch));

  const auto step {0.5f * delta_time};
  for (auto i = 0; i < color_vertices_.size(); ++i) {
    if (color_vertices_direction_[i]) {
      if (color_vertices_[i] < 1) {
        color_vertices_[i] += step * generateRand(0.f, 1.f);
      } else {
        color_vertices_[i] -= step * generateRand(0.f, 1.f);
        color_vertices_direction_[i] = !color_vertices_direction_[i];
      }
    } else {
      if (color_vertices_[i] > 0) {
        color_vertices_[i] -= step * generateRand(0.f, 1.f);
      } else {
        color_vertices_[i] += step * generateRand(0.f, 1.f);
        color_vertices_direction_[i] = !color_vertices_direction_[i];
      }
    }
  }
  //cube_.colors_.bind();
  // SDL2_GL::glBufferDataFromVector(GL_ARRAY_BUFFER, color_vertices_, GL_STATIC_DRAW);

  model_ = glm::rotate(model_, glm::radians(angle_ * delta_time), glm::vec3(0.f, 1.f, 0.f));
  shader_program_.setMat4f("model", glm::value_ptr(model_));

  view_ = glm::lookAt(camera_pos_, camera_pos_ + camera_front_, camera_up_);
  shader_program_.setMat4f("view", glm::value_ptr(view_));
}
