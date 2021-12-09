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

using GLfloatVector = std::vector<GLfloat>;
using GLuintVector = std::vector<GLuint>;

namespace SDL2_GL {
  GLenum glCheckError_(const char* file, int line);
  #define glCheckError() ktp::SDL2_GL::glCheckError_(__FILE__, __LINE__)

  template<typename T>
  inline void glBufferDataFromVector(GLenum target, const std::vector<T>& v, GLenum usage) {
    glBufferData(target, v.size() * sizeof(T), v.data(), usage);
  }

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
  ShaderProgram& operator=(const ShaderProgram& other) {
    id_ = other.id_;
    return *this;
  }
  ShaderProgram& operator=(ShaderProgram&& other) {
    if (this != &other) {
      id_ = std::exchange(other.id_, 0);
    }
    return *this;
  }
  inline auto id() const { return id_; }
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
  inline void setMat2f(const char* name, const GLfloat* value, GLboolean transpose = GL_FALSE) const {
    glUseProgram(id_);
    glUniformMatrix2fv(glGetUniformLocation(id_, name), 1, transpose, value);
  }
  inline void setMat3f(const char* name, const GLfloat* value, GLboolean transpose = GL_FALSE) const {
    glUseProgram(id_);
    glUniformMatrix3fv(glGetUniformLocation(id_, name), 1, transpose, value);
  }
  inline void setMat4f(const char* name, const GLfloat* value, GLboolean transpose = GL_FALSE) const {
    glUseProgram(id_);
    glUniformMatrix4fv(glGetUniformLocation(id_, name), 1, transpose, value);
  }
  inline void setUint(const char* name, GLuint value) const {
    glUseProgram(id_);
    glUniform1ui(glGetUniformLocation(id_, name), value);
  }
  void setup(const std::string& vertex_shader_path, const std::string& fragment_shader_path);
  inline void use() const { glUseProgram(id_); }

 private:
  static void printProgramLog(GLuint program);
  static void printShaderLog(GLuint shader);

  GLuint id_ {};
};

struct VAO_Config {
  GLsizei              stride_ {};
  GLenum               usage_ {};
  std::vector<GLfloat> vertices_ {};
};

class VBO {
  friend class VAO;
 public:
  VBO() = default;
  VBO(const VBO& other) = delete;
  VBO(VBO&& other) { *this = std::move(other); }
  ~VBO() { glDeleteBuffers(1, &id_); }
  VBO& operator=(const VBO& other) = delete;
  VBO& operator=(VBO&& other) {
    if (this != &other) {
      id_ = std::exchange(other.id_, 0);
    }
    return *this;
  }
  void bind() const { glBindBuffer(GL_ARRAY_BUFFER, id_); }
  auto id() const { return id_; }
  void setup(const GLfloatVector& vertices);
  void setup(GLfloat* vertices, GLsizeiptr size);
  void unbind() const { glBindBuffer(GL_ARRAY_BUFFER, 0); }
 private:
  GLuint id_ {};
};

class EBO {
  friend class VAO;
 public:
  EBO() = default;
  EBO(const EBO& other) = delete;
  EBO(EBO&& other) { *this = std::move(other); }
  ~EBO() { glDeleteBuffers(1, &id_); }
  EBO& operator=(const EBO& other) = delete;
  EBO& operator=(EBO&& other) {
    if (this != &other) {
      id_ = std::exchange(other.id_, 0);
    }
    return *this;
  }
  void bind() const { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id_); }
  auto id() const { return id_; }
  void setup(const GLuintVector& indices);
  void setup(GLuint* indices, GLsizeiptr size);
  void unbind() const { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); }
 private:
  GLuint id_ {};
};

class VAO {
 public:
  VAO() = default;
  VAO(const VAO& other) = delete;
  VAO(VAO&& other) { *this = std::move(other); }
  ~VAO() { glDeleteVertexArrays(1, &id_); }
  VAO& operator=(const VAO& other) = delete;
  VAO& operator=(VAO&& other) {
    if (this != &other) {
      id_ = std::exchange(other.id_, 0);
      colors_ = std::move(other.colors_);
      shape_ = std::move(other.shape_);
    }
    return *this;
  }
  void bind() const { glBindVertexArray(id_); }
  inline void draw() const {
    glBindVertexArray(id_);
    glDrawArrays(GL_TRIANGLES, 0, 36);
  }
  void setup(const VAO_Config& config);

  VBO shape_ {};
  VBO colors_ {};

 private:
  GLuint id_ {};

};

} // namespace ktp

#endif // KTP_SDL2_WRAPPERS_SDL2_OPENGL_HPP_
