#ifndef KTP_SDL2_WRAPPERS_SDL2_OPENGL_HPP_
#define KTP_SDL2_WRAPPERS_SDL2_OPENGL_HPP_

#include <SDL.h>
#include <GL/glew.h>
#include <SDL_opengl.h>
#include <GL/GLU.h>
#include <string>
#include <utility>
#include <vector>

namespace ktp {

class SDL2_Window;

namespace SDL2_GL {
  struct VAO_Config {
    std::string          fragment_shader_path_ {};
    std::vector<GLuint>  indices_ {};
    GLenum               usage_ {};
    std::string          vertex_shader_path_ {};
    std::vector<GLfloat> vertices_ {};
  };
  std::vector<GLfloat> cube(GLfloat size = 1.f);
  bool initGLEW(SDL_GLContext& context, SDL2_Window& window);
  auto loadShaders(const char* vertex_file_path, const char* fragment_file_path);
  GLuint loadShaders(const std::string& vertex_file_path, const std::string& fragment_file_path);
  void printProgramLog(GLuint program);
  void printShaderLog(GLuint shader);
  template <typename T>
  inline void glBufferDataFromVector(GLenum target, const std::vector<T>& v, GLenum usage) {
    glBufferData(target, v.size() * sizeof(T), v.data(), usage);
  }
} // namespace SDL2_GL

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

class SDL2_VAO {
 public:
  SDL2_VAO() = default;
  SDL2_VAO(const SDL2_VAO& other) = delete;
  SDL2_VAO(SDL2_VAO&& other) { *this = std::move(other); }
  ~SDL2_VAO();
  SDL2_VAO& operator=(const SDL2_VAO& other) = delete;
  SDL2_VAO& operator=(SDL2_VAO&& other) {
    if (this != &other) {
      id_ = std::exchange(other.id_, 0);
      shader_program_ = std::exchange(other.shader_program_, 0);
      vbo_ = std::exchange(other.vbo_, 0);
    }
    return *this;
  }
  inline void draw() const {
    glUseProgram(shader_program_);
    glBindVertexArray(id_);
    glDrawArrays(GL_TRIANGLES, 0, vertices_count_);
    //glDrawElements(GL_TRIANGLES, indices_count_, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
  }
  void setup(const SDL2_GL::VAO_Config& config);

 private:
  GLuint ebo_ {};
  GLuint id_ {};
  GLuint indices_count_ {};
  GLuint shader_program_ {};
  GLuint vbo_ {};
  GLuint vertices_count_ {};
};

} // namespace ktp

#endif // KTP_SDL2_WRAPPERS_SDL2_OPENGL_HPP_
