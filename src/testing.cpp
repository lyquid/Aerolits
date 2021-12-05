#include "include/paths.hpp"
#include "include/random.hpp"
#include "include/testing.hpp"
#include "sdl2_wrappers/sdl2_geometry.hpp"
#include "sdl2_wrappers/sdl2_log.hpp"
#include "sdl2_wrappers/sdl2_timer.hpp"

ktp::Testing::Testing() {}

void ktp::Testing::draw() const {
  vao1_.draw();
}

void ktp::Testing::tutorial() {
  const auto path {getResourcesPath("shaders")};
  vao1_config_.usage_ = GL_STATIC_DRAW;
  vao1_config_.vertex_shader_path_ = path + "vertex_shader.glsl";
  vao1_config_.fragment_shader_path_ = path + "fragment_shader.glsl";
  vao1_config_.vertices_ = {
    // positions         // colors
    -0.5f, -0.5f, 0.f,  1.0f, 0.0f, 0.0f,   // bottom left
     0.5f, -0.5f, 0.f,  0.0f, 1.0f, 0.0f,   // bottom right
     0.f,   0.5f, 0.f,  0.0f, 0.0f, 1.0f    // top
  };
  vao1_config_.vertex_attribute_count_ = 2;
  vao1_config_.stride_ = 6;
  vao1_.setup(vao1_config_);
}

void ktp::Testing::update(float delta_time) {
  angle_ += 0.001f;
  trans_ = glm::rotate(trans_, glm::radians(angle_), glm::vec3(0.f, 0.f, 1.f));
  vao1_.shaderProgram().setMat4f("transform", trans_);
}
