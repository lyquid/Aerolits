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
}

void ktp::Testing::init() {
  shader_program_ = Resources::getShader("test");

  vertices_data_ = {
    // positions      // colors
    -0.05f,  0.05f,   1.0f, 0.0f, 0.0f,
     0.05f, -0.05f,   0.0f, 1.0f, 0.0f,
    -0.05f, -0.05f,   0.0f, 0.0f, 1.0f,

    -0.05f,  0.05f,   1.0f, 0.0f, 0.0f,
     0.05f, -0.05f,   0.0f, 1.0f, 0.0f,
     0.05f,  0.05f,   0.0f, 1.0f, 1.0f
  };

  // vao_.bind();
  vertices_.setup(vertices_data_);
  vao_.linkAttrib(vertices_, 0, 2, GL_FLOAT, 5 * sizeof(GLfloat), nullptr);
  vao_.linkAttrib(vertices_, 1, 3, GL_FLOAT, 5 * sizeof(GLfloat), (void*)(sizeof(GLfloat) * 2));

  glm::vec2 translations[100];
  int index {0};
  float offset {0.1f};
  for (int y = -10; y < 10; y += 2) {
    for (int x = -10; x < 10; x += 2) {
      glm::vec2 translation {};
      translation.x = (float)x / 10.0f + offset;
      translation.y = (float)y / 10.0f + offset;
      translations[index++] = translation;
    }
  }

  shader_program_.use();
  for (unsigned int i = 0; i < 100; i++) {
    //shader_program_.setVec2(("offsets[" + std::to_string(i) + "]")), translations[i]);
  }
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
  shader_program_.setMat4f("mvp", glm::value_ptr(mvp));
}
