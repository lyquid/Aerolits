#include "include/box2d_utils.hpp"
#include "include/palette.hpp"
#include "include/random.hpp"
#include "include/resources.hpp"
#include "include/testing.hpp"
#include "sdl2_wrappers/sdl2_geometry.hpp"
#include "sdl2_wrappers/sdl2_log.hpp"
#include "sdl2_wrappers/sdl2_timer.hpp"

// ktp::Testing::Testing() {
//   logMessage("testing ctt");
// }

void ktp::Testing::draw() const {
  shader_program_.setMat4f("mvp", glm::value_ptr(mvp_));
  vao_.bind();
  glDrawArrays(GL_TRIANGLES, 0, 36);
}

void ktp::Testing::tutorial() {
  shader_program_ = Resources::getShader("testing");
  vertices_color_.resize(108);
  vertices_color_direction_.resize(vertices_color_.size());
  for (auto i = 0u; i < vertices_color_.size() / 3; ++i) {
    const auto color {SDL2ColorToB2Color(Colors::palette.at(generateRand(0u, Colors::palette.size() - 1)))};
    vertices_color_[3 * i + 0] = color.r;
    vertices_color_[3 * i + 1] = color.g;
    vertices_color_[3 * i + 2] = color.b;
    vertices_color_direction_[3 * i + 0] = generateRand(0u, 1u);
    vertices_color_direction_[3 * i + 1] = generateRand(0u, 1u);
    vertices_color_direction_[3 * i + 2] = generateRand(0u, 1u);
  }
  vertices_ = SDL2_GL::cube(0.5f);

  vbo_.setup(vertices_);
  vao_.linkAttrib(vbo_, 0, 3, GL_FLOAT, 0, nullptr);
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
  for (auto i = 0; i < vertices_color_.size(); ++i) {
    if (vertices_color_direction_[i]) {
      if (vertices_color_[i] < 1) {
        vertices_color_[i] += step * generateRand(0.f, 1.f);
      } else {
        vertices_color_[i] -= step * generateRand(0.f, 1.f);
        vertices_color_direction_[i] = !vertices_color_direction_[i];
      }
    } else {
      if (vertices_color_[i] > 0) {
        vertices_color_[i] -= step * generateRand(0.f, 1.f);
      } else {
        vertices_color_[i] += step * generateRand(0.f, 1.f);
        vertices_color_direction_[i] = !vertices_color_direction_[i];
      }
    }
  }
  //cube_.colors_.bind();
  // SDL2_GL::glBufferDataFromVector(GL_ARRAY_BUFFER, color_vertices_, GL_STATIC_DRAW);

  glm::mat4 model {1.f};
  model = glm::rotate(model, glm::radians(angle_ * delta_time), glm::vec3(0.f, 1.f, 0.f));
  view_ = glm::lookAt(camera_pos_, camera_pos_ + camera_front_, camera_up_);
  mvp_ = projection_ * view_ * model;
}
