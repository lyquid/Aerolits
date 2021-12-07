#include "include/paths.hpp"
#include "include/random.hpp"
#include "include/testing.hpp"
#include "sdl2_wrappers/sdl2_geometry.hpp"
#include "sdl2_wrappers/sdl2_log.hpp"
#include "sdl2_wrappers/sdl2_timer.hpp"

ktp::Testing::Testing() {}

void ktp::Testing::draw() const {
  shader_program_.use();
  cube_.draw();
}

void ktp::Testing::tutorial() {
  const auto vertex_shader_path {getResourcesPath("shaders") + "vertex_shader.glsl"};
  const auto fragment_shader_path {getResourcesPath("shaders") + "fragment_shader.glsl"};

  cube_config_.usage_ = GL_STATIC_DRAW;
  /* cube_config_.vertices_ = {
    // positions         // colors
    -0.5f, -0.5f, 0.f,  1.0f, 0.0f, 0.0f,   // bottom left
     0.5f, -0.5f, 0.f,  0.0f, 1.0f, 0.0f,   // bottom right
     0.f,   0.5f, 0.f,  0.0f, 0.0f, 1.0f    // top
  }; */
  const std::vector<float> vertices = {
    -0.5f, -0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,
    -0.5f,  0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,

    -0.5f, -0.5f,  0.5f,
     0.5f, -0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,
    -0.5f, -0.5f,  0.5f,

    -0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,

     0.5f,  0.5f,  0.5f,
     0.5f,  0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f, -0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,

    -0.5f, -0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f, -0.5f,  0.5f,
     0.5f, -0.5f,  0.5f,
    -0.5f, -0.5f,  0.5f,
    -0.5f, -0.5f, -0.5f,

    -0.5f,  0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,
     0.5f,  0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f, -0.5f
  };
  //cube_config_.vertices_ = SDL2_GL::cube(0.5f);
  cube_config_.vertices_ = vertices;
  cube_config_.stride_ = 0;
  cube_.setup(cube_config_);

  shader_program_.setup(vertex_shader_path, fragment_shader_path);

  model_ = glm::rotate(model_, glm::radians(-55.f), glm::vec3(1.f, 0.f, 0.f));
  view_ = glm::translate(view_, glm::vec3(0.f, 0.f, -3.f));
  projection_ = glm::perspective(glm::radians(45.f), 1366.f / 768.f, 0.1f, 100.f);

  shader_program_.setMat4f("model", glm::value_ptr(model_));
  shader_program_.setMat4f("view", glm::value_ptr(view_));
  shader_program_.setMat4f("projection", glm::value_ptr(projection_));
}

void ktp::Testing::update(float delta_time) {
  model_ = glm::rotate(model_, glm::radians(angle_ * delta_time), glm::vec3(0.f, 0.f, 1.f));
  shader_program_.setMat4f("model", glm::value_ptr(model_));
}
