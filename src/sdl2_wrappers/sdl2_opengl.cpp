#include "sdl2_log.hpp"
#include "sdl2_opengl.hpp"
#include <fstream>
#include <sstream>
#include <vector>

bool ktp::SDL2_GLEW::init(SDL_GLContext& context, SDL_Window* window) {
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
  SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
  glEnable(GL_MULTISAMPLE);

  context = SDL_GL_CreateContext(window);
  if (!context) {
    logSDL2Error("SDL_GL_CreateContext", SDL_LOG_CATEGORY_RENDER);
    return false;
  } else {
    // GLEW
    glewExperimental = GL_TRUE;
    const auto glew_error {glewInit()};
    if (glew_error != GLEW_OK) {
      std::stringstream error_msg {"Error initializing GLEW! "};
      error_msg << glewGetErrorString(glew_error);
      logError(error_msg.str(), SDL_LOG_CATEGORY_RENDER);
      return false;
    }
    // Vsync
    if (SDL_GL_SetSwapInterval(0) < 0) {
      logSDL2Error("SDL_GL_SetSwapInterval", SDL_LOG_CATEGORY_RENDER);
    }
  }
  return true;
}

GLuint ktp::SDL2_GLEW::loadShaders(const char* vertex_file_path, const char* fragment_file_path) {
  const std::string vfp {vertex_file_path};
  const std::string ffp {fragment_file_path};
  return loadShaders(vfp, ffp);
}

GLuint ktp::SDL2_GLEW::loadShaders(const std::string& vertex_file_path, const std::string& fragment_file_path) {
  // Create the shaders
	GLuint vertex_shader_id {glCreateShader(GL_VERTEX_SHADER)};
	GLuint fragment_shader_id {glCreateShader(GL_FRAGMENT_SHADER)};
  // Read the Vertex Shader code from the file
	std::string vertex_shader_code {};
	std::ifstream vertex_shader_stream {vertex_file_path, std::ios::in};
	if (vertex_shader_stream.is_open()) {
		std::stringstream sstr {};
		sstr << vertex_shader_stream.rdbuf();
		vertex_shader_code = sstr.str();
		vertex_shader_stream.close();
	} else {
    logError("Could NOT open vertex shader file", vertex_file_path);
		return GL_FALSE;
	}
  // Read the Fragment Shader code from the file
	std::string fragment_shader_code {};
	std::ifstream fragment_shader_stream {fragment_file_path, std::ios::in};
	if (fragment_shader_stream.is_open()) {
		std::stringstream sstr {};
		sstr << fragment_shader_stream.rdbuf();
		fragment_shader_code = sstr.str();
		fragment_shader_stream.close();
	} else {
    logError("Could NOT open fragment shader file", fragment_file_path);
		return GL_FALSE;
	}
  // Compile Vertex Shader
  logMessage("Compiling vertex shader " + vertex_file_path);
	auto vertex_source_pointer {vertex_shader_code.c_str()};
	glShaderSource(vertex_shader_id, 1, &vertex_source_pointer, nullptr);
	glCompileShader(vertex_shader_id);
  printShaderLog(vertex_shader_id);
  // Compile Fragment Shader
  logMessage("Compiling fragment shader " + fragment_file_path);
	auto fragment_source_pointer {fragment_shader_code.c_str()};
	glShaderSource(fragment_shader_id, 1, &fragment_source_pointer, nullptr);
	glCompileShader(fragment_shader_id);
  printShaderLog(fragment_shader_id);
  // Link the program
	GLuint program_id {glCreateProgram()};
	glAttachShader(program_id, vertex_shader_id);
	glAttachShader(program_id, fragment_shader_id);
	glLinkProgram(program_id);
  printProgramLog(program_id);
  // clean
	glDetachShader(program_id, vertex_shader_id);
	glDetachShader(program_id, fragment_shader_id);
	glDeleteShader(vertex_shader_id);
	glDeleteShader(fragment_shader_id);

  return program_id;
}

void ktp::SDL2_GLEW::printProgramLog(GLuint program) {
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

void ktp::SDL2_GLEW::printShaderLog(GLuint shader) {
  // Make sure name is shader
  if (glIsShader(shader)) {
    // Shader log length
    int info_log_length{}, max_length{};
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
