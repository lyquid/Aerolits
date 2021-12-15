#pragma once

#include "../sdl2_wrappers/sdl2_opengl.hpp"
#include <map>
#include <string>

namespace ktp { namespace Resources {

  using ShadersMap = std::map<std::string, GLuint>;

  extern ShadersMap shaders_map;

  void clear();
  ShaderProgram loadShader(const std::string& name, const std::string& vertex_shader_path, const std::string& fragment_shader_path, const std::string& geometry_shader_path = "");
  GLuint loadShaderFromFile(const std::string& vertex_shader_path, const std::string& fragment_shader_path, const std::string& geometry_shader_path = "");
  inline ShaderProgram getShader(const std::string& name) { return shaders_map[name]; }
  void printProgramLog(GLuint program);
  void printShaderLog(GLuint shader);

} } // namespace resources / ktp
