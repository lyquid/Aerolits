#ifndef KTP_SDL2_WRAPPERS_SDL2_OPENGL_HPP_
#define KTP_SDL2_WRAPPERS_SDL2_OPENGL_HPP_

#include <SDL.h>
#include <GL/glew.h>
#include <SDL_opengl.h>
#include <GL/GLU.h>
#include <string>
#include <vector>

namespace ktp {

class GLProgram {
 public:
  GLProgram() {}
  GLProgram(GLuint id): id_(id) {}
  GLProgram(const GLProgram& other) { *this = other; }
  ~GLProgram() { glDeleteProgram(id_); }
  GLProgram& operator=(const GLProgram& other) {
    id_ = other.id_;
    return *this;
  }
  GLProgram& operator=(GLuint id) {
    id_ = id;
    return *this;
  }
  inline auto id() const { return id_; }
  inline auto& id() { return id_; }
  inline void setId(GLuint id) { id_ = id; }
 private:
  GLuint id_ {};
};

class SDL2_GLContext {
 public:
  ~SDL2_GLContext() { if (context_) SDL_GL_DeleteContext(context_); }
  /**
   * @return A read and write reference to the SDL_GLContext.
   */
  inline auto& context() { return context_; }
 private:
  SDL_GLContext context_ {nullptr};
};

namespace SDL2_GLEW {
  std::vector<GLfloat> cube(GLfloat size = 1.f);
  bool init(SDL_GLContext& context, SDL_Window* window);
  auto loadShaders(const char* vertex_file_path, const char* fragment_file_path);
  GLuint loadShaders(const std::string& vertex_file_path, const std::string& fragment_file_path);
  void printProgramLog(GLuint program);
  void printShaderLog(GLuint shader);
  template <typename T>
  inline void glBufferDataFromVector(GLenum target, const std::vector<T>& v, GLenum usage) {
    glBufferData(target, v.size() * sizeof(T), v.data(), usage);
  }
} // namespace SDL2_GLEW

} // namespace ktp

#endif // KTP_SDL2_WRAPPERS_SDL2_OPENGL_HPP_
