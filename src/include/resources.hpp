#pragma once

#include "../sdl2_wrappers/sdl2_font.hpp"
#include "../sdl2_wrappers/sdl2_opengl.hpp"
#include <map>
#include <string>

namespace ktp { namespace Resources {

  using FontsMap    = std::map<std::string, SDL2_Font>;
  using ShadersMap  = std::map<std::string, GLuint>;
  using TexturesMap = std::map<std::string, GLuint>;

  extern FontsMap    fonts_map;
  extern ShadersMap  shaders_map;
  extern TexturesMap textures_map;

  // clean
  void clear();

  // paths
  std::string getConfigPath(const std::string& sub_dir = "");
  std::string getResourcesPath(const std::string& sub_dir = "");

  // fonts
  inline auto getFont(const std::string name) { return fonts_map.at(name).getFont(); }
  void loadFont(const std::string& name, const std::string& file, int size);

  // shaders
  inline auto getShader(const std::string& name) { return ShaderProgram{shaders_map.at(name)}; }
  void loadShader(const std::string& name, const std::string& vertex_shader_path, const std::string& fragment_shader_path, const std::string& geometry_shader_path = "");
  void printProgramLog(GLuint program);
  void printShaderLog(GLuint shader);

  // textures
  inline auto getTexture(const std::string& name) { return Texture2D{textures_map.at(name)}; }
  void loadTexture(const std::string& name, const std::string& file, bool alpha);

} } // namespace resources / ktp
