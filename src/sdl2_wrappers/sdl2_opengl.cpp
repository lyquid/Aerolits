#include "sdl2_log.hpp"
#include "sdl2_opengl.hpp"
#include "sdl2_window.hpp"
#include "../include/palette.hpp"
#include <array>
#include <fstream>
#include <sstream>

bool ktp::SDL2_GL::initGLEW(SDL2_GLContext& context, const SDL2_Window& window) {
  context.context() = SDL_GL_CreateContext(window.getWindow());
  if (!context.context()) {
    logSDL2Error("SDL_GL_CreateContext", SDL_LOG_CATEGORY_RENDER);
    return false;
  } else {
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    // glEnable(GL_CULL_FACE);
    const auto clear_color {Palette::black};
    glClearColor(clear_color.r, clear_color.g, clear_color.b, clear_color.a);
    // GLEW
    glewExperimental = GL_TRUE;
    const auto glew_error {glewInit()};
    if (glew_error != GLEW_OK) {
      std::stringstream error_msg {"Error initializing GLEW! "};
      error_msg << glewGetErrorString(glew_error);
      logError(error_msg.str(), SDL_LOG_CATEGORY_RENDER);
      return false;
    }
    // viewport
    glViewport(0, 0, window.getSize().x, window.getSize().y);
    // Vsync
    if (SDL_GL_SetSwapInterval(0) < 0) {
      logSDL2Error("SDL_GL_SetSwapInterval", SDL_LOG_CATEGORY_RENDER);
    }
    // result
    int result {}, minor {};
    SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &result);
    SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &minor);
    logMessage("OpenGL verion: " + std::to_string(result) + '.' + std::to_string(minor));
    SDL_GL_GetAttribute(SDL_GL_MULTISAMPLEBUFFERS, &result);
    logMessage("Multisample buffers: " + std::to_string(result));
    SDL_GL_GetAttribute(SDL_GL_MULTISAMPLESAMPLES, &result);
    logMessage("Multisample samples: " + std::to_string(result));
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &result);
    logMessage("Maximum vertex attributes supported: " + std::to_string(result), SDL_LOG_CATEGORY_RENDER);
  }
  return true;
}
