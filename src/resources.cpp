#include "include/resources.hpp"
#include "sdl2_wrappers/sdl2_log.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <filesystem>
#include <fstream>
#include <sstream>
#include <utility>

ktp::Resources::FontsMap    ktp::Resources::fonts_map {};
ktp::Resources::ShadersMap  ktp::Resources::shaders_map {};
ktp::Resources::TexturesMap ktp::Resources::textures_map {};

void ktp::Resources::cleanOpenGL() {
  for (auto& [name, shader_id]: shaders_map) {
    glDeleteProgram(shader_id);
  }
  for (auto& [name, texture_id]: textures_map) {
    glDeleteTextures(1, &texture_id);
  }
}

/* PATHS */

std::string ktp::Resources::getConfigPath(const std::string& sub_dir) {
  const std::string kPROJECT_NAME = "Aerolits";
  constexpr auto kCONFIG_FOLDER = "config";

  #ifdef _WIN32
    constexpr auto kPATH_SEPARATOR = '\\';
  #else
    constexpr auto kPATH_SEPARATOR = '/';
  #endif

  auto path {std::filesystem::current_path().string()};
  const auto kPos {path.rfind(kPROJECT_NAME) + kPROJECT_NAME.length()};

  path = path.substr(0, kPos) + kPATH_SEPARATOR + kCONFIG_FOLDER + kPATH_SEPARATOR;
  return sub_dir.empty() ? path : path + sub_dir + kPATH_SEPARATOR;
}

std::string ktp::Resources::getResourcesPath(const std::string& sub_dir) {
  const std::string kPROJECT_NAME = "Aerolits";
  constexpr auto kRESOURCES_FOLDER = "resources";

  #ifdef _WIN32
    constexpr auto kPATH_SEPARATOR = '\\';
  #else
    constexpr auto kPATH_SEPARATOR = '/';
  #endif

  auto path {std::filesystem::current_path().string()};
  const auto kPos {path.rfind(kPROJECT_NAME) + kPROJECT_NAME.length()};

  path = path.substr(0, kPos) + kPATH_SEPARATOR + kRESOURCES_FOLDER + kPATH_SEPARATOR;
  return sub_dir.empty() ? path : path + sub_dir + kPATH_SEPARATOR;
}

/* FONTS */

void ktp::Resources::loadFont(const std::string& name, const std::string& file, int size) {
  SDL2_Font font {};
  if (font.loadFont(file, size)) {
    fonts_map[name] = std::move(font);
    logMessage("Loaded font \"" + name + "\" (" + std::to_string(size) + ") from file " + file);
  }
}

/* SHADERS */

bool ktp::Resources::loadShader(const std::string& name, const std::string& vertex_shader_path, const std::string& fragment_shader_path, const std::string& geometry_shader_path) {
  // Create the shaders
	GLuint vertex_shader_id {glCreateShader(GL_VERTEX_SHADER)};
	GLuint fragment_shader_id {glCreateShader(GL_FRAGMENT_SHADER)};
  bool geometry_shader_present {};
  GLuint geometry_shader_id {};
  if (geometry_shader_path != "") {
    geometry_shader_present = true;
    glCreateShader(GL_GEOMETRY_SHADER);
  }
  // Read the Vertex Shader code from the file
	std::string vertex_shader_code {};
	std::ifstream vertex_shader_stream {vertex_shader_path, std::ios::in};
	if (vertex_shader_stream.is_open()) {
		std::stringstream sstr {};
		sstr << vertex_shader_stream.rdbuf();
		vertex_shader_code = sstr.str();
		vertex_shader_stream.close();
	} else {
    logError("Could NOT open vertex shader file", vertex_shader_path);
    return false;
	}
  // Read the Fragment Shader code from the file
	std::string fragment_shader_code {};
	std::ifstream fragment_shader_stream {fragment_shader_path, std::ios::in};
	if (fragment_shader_stream.is_open()) {
		std::stringstream sstr {};
		sstr << fragment_shader_stream.rdbuf();
		fragment_shader_code = sstr.str();
		fragment_shader_stream.close();
	} else {
    logError("Could NOT open fragment shader file", fragment_shader_path);
    return false;
	}
  std::string geometry_shader_code {};
  if (geometry_shader_present) {
    // Read the Geometry Shader code from the file
    std::ifstream geometry_shader_stream {geometry_shader_path, std::ios::in};
    if (geometry_shader_stream.is_open()) {
      std::stringstream sstr {};
      sstr << geometry_shader_stream.rdbuf();
      geometry_shader_code = sstr.str();
      geometry_shader_stream.close();
    } else {
      logError("Could NOT open geometry shader file", geometry_shader_path);
      return false;
    }
  }
  // Compile Vertex Shader
  logMessage("Compiling vertex shader " + vertex_shader_path);
	const auto vertex_source_pointer {vertex_shader_code.c_str()};
	glShaderSource(vertex_shader_id, 1, &vertex_source_pointer, nullptr);
	glCompileShader(vertex_shader_id);
  glCheckError();
  if (!printShaderLog(vertex_shader_id)) return false;
  // Compile Fragment Shader
  logMessage("Compiling fragment shader " + fragment_shader_path);
	const auto fragment_source_pointer {fragment_shader_code.c_str()};
	glShaderSource(fragment_shader_id, 1, &fragment_source_pointer, nullptr);
	glCompileShader(fragment_shader_id);
  glCheckError();
  if (!printShaderLog(fragment_shader_id)) return false;
  // Compile Geometry shader
  if (geometry_shader_present) {
    logMessage("Compiling geometry shader " + geometry_shader_path);
    const auto geometry_source_pointer {geometry_shader_code.c_str()};
    glShaderSource(geometry_shader_id, 1, &geometry_source_pointer, nullptr);
    glCompileShader(geometry_shader_id);
    glCheckError();
    if (!printShaderLog(geometry_shader_id)) return false;
  }
  // Link the program
	const GLuint id {glCreateProgram()};
  glCheckError();
	glAttachShader(id, vertex_shader_id);
  glCheckError();
	glAttachShader(id, fragment_shader_id);
  glCheckError();
  if (geometry_shader_present) {
    glAttachShader(id, geometry_shader_id);
    glCheckError();
  }
	glLinkProgram(id);
  glCheckError();
  if (!printProgramLog(id)) return false;
  // Clean
	glDeleteShader(vertex_shader_id);
  glCheckError();
	glDeleteShader(fragment_shader_id);
  glCheckError();
  if (geometry_shader_present) {
    glDeleteShader(geometry_shader_id);
    glCheckError();
  }

  shaders_map[name] = id;
  logMessage("Shader program \"" + name + "\" successfully compiled and linked.");
  return true;
}

bool ktp::Resources::printProgramLog(GLuint program) {
  // Make sure name is program
  if (glIsProgram(program)) {
    // Program log length
    int info_log_length{}, max_length{};
    // Get info string length
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &max_length);
    std::string info_log {};
    info_log.resize(max_length);
    // Get info log
    glGetProgramInfoLog(program, max_length, &info_log_length, info_log.data());
    if (info_log_length > 0) {
      logError(info_log);
      return false;
    }
  } else {
    logError("printProgramLog(): Name " + std::to_string(program) + " is not a program.");
    return false;
  }
  return true;
}

bool ktp::Resources::printShaderLog(GLuint shader) {
  // Make sure name is shader
  if (glIsShader(shader)) {
    // Shader log length
    int info_log_length {}, max_length {};
    // Get info string length
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &max_length);
    std::string info_log {};
    info_log.resize(max_length);
    // Get info log
    glGetShaderInfoLog(shader, max_length, &info_log_length, info_log.data());
    if (info_log_length > 0) {
      logError(info_log);
      return false;
    }
  } else {
    logError("printShaderLog(): Name " + std::to_string(shader) + " is not a shader.");
    return false;
  }
  return true;
}

/* TEXTURES */

void ktp::Resources::loadTexture(const std::string& name, const std::string& file, bool alpha) {
  GLuint id {};
  GLsizei width {}, height {};
  GLint internal_format {GL_RGB};
  GLenum image_format {GL_RGB};
  constexpr GLint wrap_s {GL_REPEAT}, wrap_t {GL_REPEAT};
  constexpr GLint filter_max {GL_LINEAR}, filter_min {GL_LINEAR_MIPMAP_LINEAR};
  int num_channels {};

  if (alpha) {
    internal_format = GL_RGBA;
    image_format = GL_RGBA;
  }

  stbi_set_flip_vertically_on_load(true);
  auto data {stbi_load(file.c_str(), &width, &height, &num_channels, 0)};
  if (data) {
    glGenTextures(1, &id);
    glCheckError();
    glBindTexture(GL_TEXTURE_2D, id);
    // set Texture wrap and filter modes
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap_s);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap_t);
    // When MINifying the image, use a LINEAR blend of two mipmaps, each filtered LINEARLY too (GL_LINEAR_MIPMAP_LINEAR)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter_min);
    // When MAGnifying the image (no bigger mipmap available), use XXXX filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter_max);
    // upload the img data
    glTexImage2D(GL_TEXTURE_2D, 0, internal_format, width, height, 0, image_format, GL_UNSIGNED_BYTE, data);
    glCheckError();
    glGenerateMipmap(GL_TEXTURE_2D);
    glCheckError();
    // unbind texture
    glBindTexture(GL_TEXTURE_2D, 0);
    // free the image
    stbi_image_free(data);
    logMessage("Loaded texture \"" + name + "\" from file " + file);
  } else {
    logError("Could NOT load texture " + file);
  }
  textures_map[name] = id;
}

ktp::Texture2D ktp::Resources::loadTextureFromTextBlended(const std::string& name, const std::string& text, const std::string& font, SDL_Color color) {
  // if already exists, delete the texture
  if (textures_map.count(name)) glDeleteTextures(1, &textures_map[name]);
  // render a surface, remember to free it when done
  SDL_Surface* surface {TTF_RenderUTF8_Blended(Resources::getFont(font), text.c_str(), color)};
  if (surface) {
    GLuint id {};
    constexpr GLint internal_format {GL_RGBA};
    GLenum format {};
    constexpr GLint wrap_s {GL_REPEAT}, wrap_t {GL_REPEAT};
    constexpr GLint filter_max {GL_LINEAR}, filter_min {GL_NEAREST};
    // find out the format
    surface->format->Rmask == 255u ? format = GL_RGBA : format = GL_BGRA;
    // lets generate the opengl texture
    glGenTextures(1, &id);
    glCheckError();
    glBindTexture(GL_TEXTURE_2D, id);
    // set Texture wrap and filter modes
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap_s);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap_t);
    // When MINifying the image
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter_min);
    // When MAGnifying the image (no bigger mipmap available)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter_max);
    // upload the surface data
    glTexImage2D(GL_TEXTURE_2D, 0, internal_format, surface->w, surface->h, 0, format, GL_UNSIGNED_BYTE, surface->pixels);
    glCheckError();
    // unbind texture
    glBindTexture(GL_TEXTURE_2D, 0);
    // push it to the textures map
    Resources::textures_map[name] = id;
    // free the surface
    SDL_FreeSurface(surface);
    return Texture2D{id};
  } else {
    logSDL2Error("TTF_RenderUTF8_Blended");
    return 0;
  }
}

ktp::Texture2D ktp::Resources::loadTextureFromTextSolid(const std::string& name, const std::string& text, const std::string& font, SDL_Color color) {
  // if already exists, delete the texture
  if (textures_map.count(name)) glDeleteTextures(1, &textures_map[name]);
  // render a surface, remember to free it when done
  SDL_Surface* surface {TTF_RenderUTF8_Solid(Resources::getFont(font), text.c_str(), color)};
  if (surface) {
    GLuint id {};
    constexpr GLint internal_format {GL_RGB};
    GLenum format {};
    constexpr GLint wrap_s {GL_REPEAT}, wrap_t {GL_REPEAT};
    constexpr GLint filter_max {GL_LINEAR}, filter_min {GL_NEAREST};
    // find out the image format
    surface->format->Rmask == 255u ? format = GL_RGB : format = GL_BGR;
    // lets generate the opengl texture
    glGenTextures(1, &id);
    glCheckError();
    glBindTexture(GL_TEXTURE_2D, id);
    // set Texture wrap and filter modes
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap_s);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap_t);
    // When MINifying the image
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter_min);
    // When MAGnifying the image (no bigger mipmap available)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter_max);
    // upload the surface data
    glTexImage2D(GL_TEXTURE_2D, 0, internal_format, surface->w, surface->h, 0, format, GL_UNSIGNED_BYTE, surface->pixels);
    glCheckError();
    // unbind texture
    glBindTexture(GL_TEXTURE_2D, 0);
    // push it to the textures map
    Resources::textures_map[name] = id;
    // free the surface
    SDL_FreeSurface(surface);
    return Texture2D{id};
  } else {
    logSDL2Error("TTF_RenderUTF8_Solid");
    return 0;
  }
}
