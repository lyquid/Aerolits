#ifndef KTP_SDL2_WRAPPERS_SDL2_OPENGL_HPP_
#define KTP_SDL2_WRAPPERS_SDL2_OPENGL_HPP_

#include <SDL.h>
#include <GL/glew.h>
#include <SDL_opengl.h>
#include <GL/GLU.h>

namespace ktp {

class SDL2_GLContext {
 public:
  ~SDL2_GLContext() { SDL_GL_DeleteContext(context_); }
  /**
   * @brief
   * @return The SDL_GLContext context.
   */
  inline auto context() const { return context_; }

  /**
   * @brief
   * @return A read and write reference to the SDL_GLContext.
   */
  inline auto& context() { return context_; }
 private:
  SDL_GLContext context_ {nullptr};
};

namespace SDL2_GLEW {
  bool init(SDL_GLContext& context, SDL_Window* window);
  void printProgramLog(GLuint program);
  void printShaderLog(GLuint shader);
} // namespace SDL2_GLEW

} // namespace ktp

#endif // KTP_SDL2_WRAPPERS_SDL2_OPENGL_HPP_
