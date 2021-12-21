#pragma once

#include "../sdl2_wrappers/sdl2_opengl.hpp"
#include <map>
#include <string>

namespace ktp { namespace Resources {

  using ShadersMap  = std::map<std::string, GLuint>;
  using TexturesMap = std::map<std::string, GLuint>;

  extern ShadersMap  shaders_map;
  extern TexturesMap textures_map;

  // clean
  void clear();
  // paths
  std::string getConfigPath(const std::string& sub_dir = "");
  std::string getResourcesPath(const std::string& sub_dir = "");
  // shaders
  ShaderProgram loadShader(const std::string& name, const std::string& vertex_shader_path, const std::string& fragment_shader_path, const std::string& geometry_shader_path = "");
  GLuint loadShaderFromFile(const std::string& vertex_shader_path, const std::string& fragment_shader_path, const std::string& geometry_shader_path = "");
  inline auto getShader(const std::string& name) { return ShaderProgram{shaders_map.at(name)}; }
  void printProgramLog(GLuint program);
  void printShaderLog(GLuint shader);
  // textures
  Texture2D loadTexture(const std::string& name, const std::string& file, bool alpha);
  GLuint loadTextureFromFile(const std::string& file, bool alpha);
  inline auto getTexture(const std::string& name) { return Texture2D{textures_map.at(name)}; }

} } // namespace resources / ktp
