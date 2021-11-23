#include "sdl2_log.hpp"
#include "sdl2_opengl.hpp"
#include <sstream>

bool ktp::SDL2_GLEW::init(SDL_GLContext& context, SDL_Window* window) {
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

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

void ktp::SDL2_GLEW::printProgramLog(GLuint program) {
  // Make sure name is shader
  if (glIsProgram(program)) {
    // Program log length
    int info_log_length{}, max_length{};
    // Get info string length
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &max_length);
    // Allocate string
    char* info_log = new char[max_length];
    // Get info log
    glGetProgramInfoLog(program, max_length, &info_log_length, info_log);
    if (info_log_length > 0) logMessage(info_log);
    // Deallocate string
    delete[] info_log;
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
    // Allocate string
    char* info_log = new char[max_length];
    // Get info log
    glGetShaderInfoLog(shader, max_length, &info_log_length, info_log);
    if(info_log_length > 0) logMessage(info_log);
    // Deallocate string
    delete[] info_log;
  } else {
    logError("printShaderLog(): Name " + std::to_string(shader) + " is not a shader.");
  }
}
