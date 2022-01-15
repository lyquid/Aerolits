#include "include/aerolite.hpp"
#include "include/box2d_utils.hpp"
#include "include/palette.hpp"
#include "include/random.hpp"
#include "include/resources.hpp"
#include "include/testing.hpp"
#include "imgui.h"
#include "sdl2_wrappers/sdl2_geometry.hpp"
#include "sdl2_wrappers/sdl2_log.hpp"
#include "sdl2_wrappers/sdl2_timer.hpp"

void ktp::Testing::draw() {
  shader_program_.use();
  vao_.bind();
  glDrawArraysInstanced(GL_TRIANGLES, 0, vertices_data_.size(), 1000);
}

void ktp::Testing::init() {
  shader_program_ = Resources::getShader("test");
  // vertices
  vertices_data_ = SDL2_GL::cube(0.05f);
  vertices_.setup(vertices_data_);
  vao_.linkAttrib(vertices_, 0, 3, GL_FLOAT, 0, nullptr);
  // colors
  GLfloatVector colors_data_ {};
  colors_data_.resize(vertices_data_.size());
  for (auto& color_c: colors_data_) {
    color_c = generateRand(0.f, 1.f);
  }
  colors_.setup(colors_data_);
  vao_.linkAttrib(colors_, 1, 3, GL_FLOAT, 0, nullptr);
  // translations
  glm::vec3 translations[1000];
  int index {0};
  float offset {0.1f};
  for (int z = -10; z < 10; z += 2) {
    for (int y = -10; y < 10; y += 2) {
      for (int x = -10; x < 10; x += 2) {
        glm::vec3 translation {};
        translation.x = (float)x / 10.0f + offset;
        translation.y = (float)y / 10.0f + offset;
        translation.z = (float)z / 10.0f + offset;
        translations[index++] = translation;
      }
    }
  }
  translations_.setup(translations, 1000 * sizeof(glm::vec3));
  vao_.linkAttrib(translations_, 2, 3, GL_FLOAT, 0, nullptr);
  glVertexAttribDivisor(2, 1);
}

void ktp::Testing::update(float delta_time) {
  updateCamera(delta_time);
  updateMVP(delta_time);
}

void ktp::Testing::updateCamera(float delta_time) {
  if (ImGui::GetIO().WantCaptureKeyboard) return;
  const auto state {SDL_GetKeyboardState(nullptr)};
  if (state[SDL_SCANCODE_W]) {
    camera_.move(CameraMovement::Forward, delta_time);
  }
  if (state[SDL_SCANCODE_S]) {
    camera_.move(CameraMovement::Backward, delta_time);
  }
  if (state[SDL_SCANCODE_A]) {
    camera_.move(CameraMovement::Left, delta_time);
  }
  if (state[SDL_SCANCODE_D]) {
    camera_.move(CameraMovement::Right, delta_time);
  }
}

void ktp::Testing::updateMVP(float delta_time) {
  glm::mat4 model {1.f};
  const glm::mat4 mvp {camera_.projectionMatrix() * camera_.viewMatrix() * model};
  shader_program_.use();
  shader_program_.setMat4f("mvp", glm::value_ptr(mvp));
}
