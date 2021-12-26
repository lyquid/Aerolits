#include "include/resources.hpp"
#include "sdl2_wrappers/sdl2_log.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
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

  constexpr auto kProjectName = "Aerolits";
  constexpr auto kConfigFolder = "config";

  #ifdef _WIN32
    constexpr auto kPathSeparator = '\\';
  #else
    constexpr auto kPathSeparator = '/';
  #endif

  std::string base_res {};

  const auto base_path = SDL_GetBasePath();

  if (base_path) {
    base_res = base_path;
    SDL_free(base_path); // <--- this IS needed. Read docs.
  } else {
    logSDL2Error("SDL_GetBasePath");
    return std::string{};
  }

  /* #ifdef _DEBUG
    size_t pos = base_res.rfind("Debug");
  #else
    size_t pos = base_res.rfind("Release");
  #endif  */
  // auto pos = base_res.rfind("bin");

  const auto pos = base_res.rfind(kProjectName) + SDL_strlen(kProjectName);
  base_res = base_res.substr(0, pos) + kPathSeparator + kConfigFolder + kPathSeparator;

  return sub_dir.empty() ? base_res : base_res + sub_dir + kPathSeparator;
}

std::string ktp::Resources::getResourcesPath(const std::string& sub_dir) {
  /* Original idea from Will Usher */
  /* Check it here: https://github.com/Twinklebear/TwinklebearDev-Lessons */
  constexpr auto kPROJECT_NAME = "Aerolits";
  constexpr auto kRESOURCES_FOLDER = "resources";

  #ifdef _WIN32
    constexpr auto kPATH_SEPARATOR = '\\';
  #else
    constexpr auto kPATH_SEPARATOR = '/';
  #endif

  std::string base_res{};

  if (base_res.empty()) {

    const auto base_path = SDL_GetBasePath();

    if (base_path) {
      base_res = base_path;
      SDL_free(base_path);
    } else {
      logSDL2Error("SDL_GetBasePath");
      return std::string();
    }

    /* #ifdef _DEBUG
      size_t pos = base_res.rfind("Debug");
    #else
      size_t pos = base_res.rfind("Release");
    #endif  */
    // auto pos = base_res.rfind("bin");

    const auto pos = base_res.rfind(kPROJECT_NAME) + SDL_strlen(kPROJECT_NAME);
    base_res = base_res.substr(0, pos) + kPATH_SEPARATOR + kRESOURCES_FOLDER + kPATH_SEPARATOR;
  }
  return sub_dir.empty() ? base_res : base_res + sub_dir + kPATH_SEPARATOR;
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

void ktp::Resources::loadShader(const std::string& name, const std::string& vertex_shader_path, const std::string& fragment_shader_path, const std::string& geometry_shader_path) {
  // Create the shaders
	GLuint vertex_shader_id {glCreateShader(GL_VERTEX_SHADER)};
	GLuint fragment_shader_id {glCreateShader(GL_FRAGMENT_SHADER)};
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
    return;
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
    return;
	}
  // Compile Vertex Shader
  logMessage("Compiling vertex shader " + vertex_shader_path);
	const auto vertex_source_pointer {vertex_shader_code.c_str()};
	glShaderSource(vertex_shader_id, 1, &vertex_source_pointer, nullptr);
	glCompileShader(vertex_shader_id);
  glCheckError();
  printShaderLog(vertex_shader_id);
  // Compile Fragment Shader
  logMessage("Compiling fragment shader " + fragment_shader_path);
	const auto fragment_source_pointer {fragment_shader_code.c_str()};
	glShaderSource(fragment_shader_id, 1, &fragment_source_pointer, nullptr);
	glCompileShader(fragment_shader_id);
  glCheckError();
  printShaderLog(fragment_shader_id);
  // Link the program
	const GLuint id {glCreateProgram()};
  glCheckError();
	glAttachShader(id, vertex_shader_id);
	glAttachShader(id, fragment_shader_id);
	glLinkProgram(id);
  printProgramLog(id);
  glCheckError();
  // clean
	// glDetachShader(id_, vertex_shader_id);
	// glDetachShader(id_, fragment_shader_id);
	glDeleteShader(vertex_shader_id);
	glDeleteShader(fragment_shader_id);
  glCheckError();

  if (id) {
    shaders_map[name] = id;
    logMessage("Shader program \"" + name + "\" successfully compiled and linked.");
  }
}

void ktp::Resources::printProgramLog(GLuint program) {
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
    if (info_log_length > 0) logMessage(info_log);
  } else {
    logError("printProgramLog(): Name " + std::to_string(program) + " is not a program.");
  }
}

void ktp::Resources::printShaderLog(GLuint shader) {
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
    if (info_log_length > 0) logMessage(info_log);
  } else {
    logError("printShaderLog(): Name " + std::to_string(shader) + " is not a shader.");
  }
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
  unsigned char* data {stbi_load(file.c_str(), &width, &height, &num_channels, 0)};
  if (data) {
    logMessage("Loaded texture \"" + name + "\" from file " + file);
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

    glTexImage2D(GL_TEXTURE_2D, 0, internal_format, width, height, 0, image_format, GL_UNSIGNED_BYTE, data);
    glCheckError();
    glGenerateMipmap(GL_TEXTURE_2D);
    glCheckError();
    // unbind texture
    glBindTexture(GL_TEXTURE_2D, 0);

    stbi_image_free(data);
  } else {
    logError("Could NOT load texture " + file);
  }
  textures_map[name] = id;
}

void ktp::Resources::loadTextureFromTextBlended(const std::string& name, const std::string& text, const std::string& font, SDL_Color color) {
  // render a surface, remember to free it when done
  SDL_Surface* surface {TTF_RenderUTF8_Blended(Resources::getFont(font), text.c_str(), color)};
  if (surface) {
    logMessage("Loaded texture \"" + name + "\" from font \"" + font + "\"");
    GLuint id {};
    GLint internal_format {};
    GLenum image_format {};
    constexpr GLint wrap_s {GL_REPEAT}, wrap_t {GL_REPEAT};
    constexpr GLint filter_max {GL_LINEAR}, filter_min {GL_NEAREST};
    const auto bytesPerPixel {surface->format->BytesPerPixel};
    // find out the image format
    if (bytesPerPixel == 4u) {    // alpha
      internal_format = GL_RGBA;
      if (surface->format->Rmask == 255u) {
        image_format = GL_RGBA;
      } else {
        image_format = GL_BGRA;
      }
    } else {                      // no alpha
      internal_format = GL_RGB;
      if (surface->format->Rmask == 255u) {
        image_format = GL_RGB;
      } else {
        image_format = GL_BGR;
      }
    }
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
    glTexImage2D(GL_TEXTURE_2D, 0, internal_format, surface->w, surface->h, 0, image_format, GL_UNSIGNED_BYTE, surface->pixels);
    glCheckError();
    // unbind texture
    glBindTexture(GL_TEXTURE_2D, 0);
    // push it to the textures map
    Resources::textures_map[name] = id;
    // free the surface
    SDL_FreeSurface(surface);
  } else {
    logSDL2Error("TTF_RenderUTF8_Blended");
  }
}
