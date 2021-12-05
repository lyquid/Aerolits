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
  GLenum glCheckError_(const char* file, int line);
  #define glCheckError() ktp::SDL2_GL::glCheckError_(__FILE__, __LINE__)

  std::vector<GLfloat> cube(GLfloat size = 1.f);

  bool initGLEW(SDL_GLContext& context, SDL2_Window& window);
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

class ShaderProgram {
 public:
  ShaderProgram() = default;
  ShaderProgram(const std::string& vertex_shader_path, const std::string& fragment_shader_path);
  ShaderProgram(const ShaderProgram& other) { *this = other; }
  ShaderProgram(ShaderProgram&& other) { *this = std::move(other); }
  ~ShaderProgram() { glDeleteProgram(id_); }
  ShaderProgram& ShaderProgram::operator=(const ShaderProgram& other) {
    id_ = other.id_;
    return *this;
  }
  ShaderProgram& ShaderProgram::operator=(ShaderProgram&& other) {
    if (this != &other) {
      id_ = std::exchange(other.id_, 0);
    }
    return *this;
  }
  inline void setBool(const char* name, bool value) const {
    glUseProgram(id_);
    glUniform1i(glGetUniformLocation(id_, name), (int)value);
  }
  inline void setInt(const char* name, GLint value) const {
    glUseProgram(id_);
    glUniform1i(glGetUniformLocation(id_, name), value);
  }
  inline void setFloat(const char* name, GLfloat value) const {
    glUseProgram(id_);
    glUniform1f(glGetUniformLocation(id_, name), value);
  }
  inline void setUint(const char* name, GLuint value) const {
    glUseProgram(id_);
    glUniform1ui(glGetUniformLocation(id_, name), value);
  }
  void setup(const std::string& vertex_shader_path, const std::string& fragment_shader_path);
  inline void use() const { glUseProgram(id_); }

 private:
  void printProgramLog();
  void printShaderLog(GLuint shader);
  GLuint id_ {};
};

struct VAO_Config {
  std::string          fragment_shader_path_ {};
  std::vector<GLuint>  indices_ {};
  GLsizei              stride_ {};
  GLenum               usage_ {};
  GLuint               vertex_attribute_count_ {};
  std::string          vertex_shader_path_ {};
  std::vector<GLfloat> vertices_ {};
};

class VAO {
 public:
  VAO() = default;
  VAO(const VAO& other) = delete;
  VAO(VAO&& other) { *this = std::move(other); }
  ~VAO();
  VAO& operator=(const VAO& other) = delete;
  VAO& operator=(VAO&& other) {
    if (this != &other) {
      ebo_ = std::exchange(other.ebo_, 0);
      id_ = std::exchange(other.id_, 0);
      indices_count_ = other.indices_count_;
      shader_program_ = std::move(other.shader_program_);
      vbo_ = std::exchange(other.vbo_, 0);
      vertices_count_ = other.vertices_count_;
    }
    return *this;
  }
  inline void draw() const {
    shader_program_.use();
    glBindVertexArray(id_);
    glDrawArrays(GL_TRIANGLES, 0, vertices_count_);
    // glDrawElements(GL_TRIANGLES, indices_count_, GL_UNSIGNED_INT, 0);
    // glBindVertexArray(0);
  }
  template<typename T>
  inline void glBufferDataFromVector(GLenum target, const std::vector<T>& v, GLenum usage) {
    glBufferData(target, v.size() * sizeof(T), v.data(), usage);
    glCheckError();
  }
  void setup(const VAO_Config& config);
  auto& shaderProgram() { return shader_program_; }

 private:
  GLuint ebo_ {};
  GLuint id_ {};
  GLuint indices_count_ {}; // can we move this to vao config??
  ShaderProgram shader_program_ {};
  GLuint vbo_ {};
  GLuint vertices_count_ {};
};

} // namespace ktp

#endif // KTP_SDL2_WRAPPERS_SDL2_OPENGL_HPP_
