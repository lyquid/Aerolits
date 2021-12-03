#include "include/paths.hpp"
#include "include/random.hpp"
#include "include/testing.hpp"
#include "sdl2_wrappers/sdl2_geometry.hpp"
#include "sdl2_wrappers/sdl2_log.hpp"

ktp::Testing::Testing() {}

void ktp::Testing::draw() const {
  vao1_.draw();
  vao2_.draw();
}

void ktp::Testing::tutorial() {
  const auto path {getResourcesPath("shaders")};
  vao1_config_.usage_ = GL_STATIC_DRAW;
  vao1_config_.vertex_shader_path_ = path + "vertex_shader.glsl";
  vao1_config_.fragment_shader_path_ = path + "fragment_shader.glsl";
  vao1_config_.vertices_ = {
    -0.5f, -0.5f, 0.f,
     0.0f, -0.5f, 0.f,
    -0.25f, 0.5f, 0.f,
  };
  vao1_.setup(vao1_config_);

  vao2_config_.usage_ = GL_STATIC_DRAW;
  vao2_config_.vertex_shader_path_ = path + "vertex_shader.glsl";
  vao2_config_.fragment_shader_path_ = path + "fragment_shader2.glsl";
  vao2_config_.vertices_ = {
    0.0f, -0.5f, 0.f,
    0.5f, -0.5f, 0.f,
    0.25f, 0.5f, 0.f
  };
  vao2_.setup(vao2_config_);
}

void ktp::Testing::update(float delta_time) {}
