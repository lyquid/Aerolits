#include "include/resources.hpp"
#include "sdl2_wrappers/sdl2_log.hpp"
#include <fstream>
#include <sstream>

ktp::Resources::ShadersMap ktp::Resources::shaders_map {};

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

void ktp::Resources::clear() {
  for (auto& [name, shader_id]: shaders_map) {
    glDeleteProgram(shader_id);
  }
}

ktp::ShaderProgram ktp::Resources::loadShader(const std::string& name, const std::string& vertex_shader_path, const std::string& fragment_shader_path, const std::string& geometry_shader_path) {
  shaders_map[name] = loadShaderFromFile(vertex_shader_path, fragment_shader_path, geometry_shader_path);
  return ShaderProgram{shaders_map[name]};
}

GLuint ktp::Resources::loadShaderFromFile(const std::string& vertex_shader_path, const std::string& fragment_shader_path, const std::string& geometry_shader_path) {
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
  return id;
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
