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
  // vao1_.shaderProgram().setFloat("offset", -0.5);
}

void ktp::Testing::update(float delta_time) {
  // const auto time {(float)SDL2_Timer::SDL2Ticks()};
  // const auto green_value {SDL_sinf(time) / 2.f + 0.5f};
  // const auto vertex_color_location {glGetUniformLocation(vao1_.shaderProgram(), "uni_color")};
  // glUseProgram(vao1_.shaderProgram());
  // glUniform4f(vertex_color_location, 0.0f, green_value, 0.0f, 1.0f);
}
