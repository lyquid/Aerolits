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

class SDL2_GLContext;
class SDL2_Window;

using GLfloatVector = std::vector<GLfloat>;
using GLuintVector  = std::vector<GLuint>;

namespace SDL2_GL {
  GLenum glCheckError_(const char* file, int line);
  #define glCheckError() ktp::SDL2_GL::glCheckError_(__FILE__, __LINE__ - 1)

  std::vector<GLfloat> cube(GLfloat size = 1.f);

  bool initGLEW(SDL2_GLContext& context, const SDL2_Window& window);
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
  ShaderProgram(GLuint id): id_(id) {}
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
  inline void setFloat4(const char* name, const GLfloat* value) const {
    glUseProgram(id_);
    glUniform4f(glGetUniformLocation(id_, name), value[0], value[1], value[2], value[3]);
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
  inline void use() const { glUseProgram(id_); }

 private:
  GLuint id_ {};
};

class VBO {
 public:
  VBO();
  VBO(const VBO& other) = delete;
  VBO(VBO&& other) { *this = std::move(other); }
  ~VBO() { if (id_) glDeleteBuffers(1, &id_); }
  VBO& operator=(const VBO& other) = delete;
  VBO& operator=(VBO&& other) {
    if (this != &other) {
      if (id_) glDeleteBuffers(1, &id_);
      id_ = std::exchange(other.id_, 0);
    }
    return *this;
  }
  void bind() const { glBindBuffer(GL_ARRAY_BUFFER, id_); }
  void setup(const GLfloatVector& vertices);
  void setup(GLfloat* vertices, GLsizeiptr size);
  void unbind() const { glBindBuffer(GL_ARRAY_BUFFER, 0); }
 private:
  GLuint id_ {};
};

class EBO {
 public:
  EBO();
  EBO(const EBO& other) = delete;
  EBO(EBO&& other) { *this = std::move(other); }
  ~EBO() { if (id_) glDeleteBuffers(1, &id_); }
  EBO& operator=(const EBO& other) = delete;
  EBO& operator=(EBO&& other) {
    if (this != &other) {
      if (id_) glDeleteBuffers(1, &id_);
      id_ = std::exchange(other.id_, 0);
    }
    return *this;
  }
  void bind() const { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id_); }
  void setup(const GLuintVector& indices);
  void setup(GLuint* indices, GLsizeiptr size);
  void unbind() const { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); }
 private:
  GLuint id_ {};
};

class VAO {
 public:
  VAO();
  VAO(const VAO& other) = delete;
  VAO(VAO&& other) { *this = std::move(other); }
  ~VAO() { if (id_) glDeleteVertexArrays(1, &id_); }
  VAO& operator=(const VAO& other) = delete;
  VAO& operator=(VAO&& other) {
    if (this != &other) {
      if (id_) glDeleteVertexArrays(1, &id_);
      id_ = std::exchange(other.id_, 0);
    }
    return *this;
  }
  void bind() const { glBindVertexArray(id_); }
  void linkAttrib(const VBO& vbo, GLuint layout, GLuint components, GLenum type, GLsizeiptr stride, void* offset) const ;
  void unbind() const { glBindVertexArray(0); }
 private:
  GLuint id_ {};
};

class Texture2D {
 public:
  Texture2D(GLuint id): id_(id) {}
  //void generate(GLsizei width, GLsizei height, unsigned char* data);
  void bind() const { glBindTexture(GL_TEXTURE_2D, id_); }
  void unbind() const { glBindTexture(GL_TEXTURE_2D, 0); }
 private:
  GLuint id_ {};
  // GLsizei width_ {}, height_ {};
  // GLint internal_format_ {GL_RGB};
  // GLenum image_format_ {GL_RGB};
  // GLint wrap_s_ {GL_REPEAT}, wrap_t_ {GL_REPEAT};
  // GLint filter_max_ {GL_LINEAR}, filter_min_ {GL_LINEAR};
};

} // namespace ktp

#endif // KTP_SDL2_WRAPPERS_SDL2_OPENGL_HPP_
