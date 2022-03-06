#ifndef KTP_SDL2_WRAPPERS_SDL2_OPENGL_HPP_
#define KTP_SDL2_WRAPPERS_SDL2_OPENGL_HPP_

#include <SDL.h>
#include <GL/glew.h>
#include <SDL_opengl.h>
#include <string>
#include <utility>
#include <vector>

namespace ktp {

class SDL2_GLContext;
class SDL2_Window;

namespace SDL2_GL {

  /**
   * @brief Inits GLEW.
   * @param context The OpenGL context.
   * @param window The SDL window object.
   * @return True on success.
   */
  bool initGLEW(SDL2_GLContext& context, const SDL2_Window& window);
} // namespace SDL2_GL

/**
 * @brief A RAII wrapper for a SDL_GLContext pointer.
 */
class SDL2_GLContext {
 public:
  SDL2_GLContext() = default;
  SDL2_GLContext(const SDL2_GLContext& other) = delete;
  SDL2_GLContext(SDL2_GLContext&& other) { *this = std::move(other); }
  ~SDL2_GLContext() { if (context_) SDL_GL_DeleteContext(context_); }
  SDL2_GLContext& operator=(const SDL2_GLContext& other) = delete;
  SDL2_GLContext& operator=(SDL2_GLContext&& other) {
    if (this != &other) {
      if (context_) SDL_GL_DeleteContext(context_);
      context_ = std::exchange(other.context_, nullptr);
    }
    return *this;
  }
  /**
   * @return A read and write reference to the SDL_GLContext.
   */
  inline auto& context() { return context_; }

 private:
  SDL_GLContext context_ {nullptr};
};
} // namespace ktp

#endif // KTP_SDL2_WRAPPERS_SDL2_OPENGL_HPP_
