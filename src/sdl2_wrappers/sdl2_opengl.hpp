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

using GLfloatVector = std::vector<GLfloat>;
using GLuintVector  = std::vector<GLuint>;

namespace SDL2_GL {
  /**
   * @brief Checks for OpenGL errors.
   */
  GLenum glCheckError_(const char* file, int line);
  #define glCheckError() ktp::SDL2_GL::glCheckError_(__FILE__, __LINE__ - 1)

  /**
   * @brief Returns the coordinates for a 3D cube build with triangles.
   * @param size The desired size.
   * @return A std::vector filled with the coords {x, y, z, x, y, ...}
   */
  std::vector<GLfloat> cube(GLfloat size = 1.f);

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

/**
 * @brief A wrapper for an OpenGL shader program.
 */
class ShaderProgram {

 public:

  ShaderProgram() = default;
  ShaderProgram(GLuint id): id_(id) {}
  /**
   * @return The id of the shader program.
   */
  inline auto id() const { return id_; }

  /**
   * @brief Sets a boolean uniform. Uses glUniform1i()
   * @param name The name of the uniform.
   * @param value The value to be set.
   */
  inline void setBool(const char* name, bool value) const {
    glUniform1i(glGetUniformLocation(id_, name), (int)value);
  }

  /**
   * @brief Sets an int uniform. Uses glUniform1i()
   * @param name The name of the uniform.
   * @param value The value to be set.
   */
  inline void setInt(const char* name, GLint value) const {
    glUniform1i(glGetUniformLocation(id_, name), value);
  }

  /**
   * @brief Sets a float uniform. Uses glUniform1f()
   * @param name The name of the uniform.
   * @param value The value to be set.
   */
  inline void setFloat(const char* name, GLfloat value) const {
    glUniform1f(glGetUniformLocation(id_, name), value);
  }

  /**
   * @brief Sets a vec4 of floats uniform. Uses glUniform4f()
   * @param name The name of the uniform.
   * @param value A pointer to the values to be set.
   */
  inline void setFloat4(const char* name, const GLfloat* value) const {
    glUniform4f(glGetUniformLocation(id_, name), value[0], value[1], value[2], value[3]);
  }

  /**
   * @brief Sets a 2x2 matrix uniform. Uses glUniformMatrix2fv()
   * @param name The name of the uniform.
   * @param value A pointer to the values to be set.
   */
  inline void setMat2f(const char* name, const GLfloat* value, GLboolean transpose = GL_FALSE) const {
    glUniformMatrix2fv(glGetUniformLocation(id_, name), 1, transpose, value);
  }

  /**
   * @brief Sets a 3x3 matrix uniform. Uses glUniformMatrix3fv()
   * @param name The name of the uniform.
   * @param value A pointer to the values to be set.
   */
  inline void setMat3f(const char* name, const GLfloat* value, GLboolean transpose = GL_FALSE) const {
    glUniformMatrix3fv(glGetUniformLocation(id_, name), 1, transpose, value);
  }

  /**
   * @brief Sets a 4x4 matrix uniform. Uses glUniformMatrix4fv()
   * @param name The name of the uniform.
   * @param value A pointer to the values to be set.
   */
  inline void setMat4f(const char* name, const GLfloat* value, GLboolean transpose = GL_FALSE) const {
    glUniformMatrix4fv(glGetUniformLocation(id_, name), 1, transpose, value);
  }

  /**
   * @brief Sets an unsigned int uniform. Uses glUniform1ui()
   * @param name The name of the uniform.
   * @param value The value to be set.
   */
  inline void setUint(const char* name, GLuint value) const {
    glUniform1ui(glGetUniformLocation(id_, name), value);
  }

  /**
   * @brief Sets a 2 component vector uniform. Uses glUniform2fv()
   * @param name The name of the uniform.
   * @param value A pointer to the values to be set.
   */
  inline void setVec2(const char* name, const GLfloat* value) const {
    glUniform2fv(glGetUniformLocation(id_, name), 1, value);
  }

  /**
   * @brief Sets a 2 component vector uniform. Uses glUniform2f()
   * @param name The name of the uniform.
   * @param x value.
   * @param y value.
   */
  inline void setVec2(const char* name, GLfloat x, GLfloat y) const {
    glUniform2f(glGetUniformLocation(id_, name), x, y);
  }

  /**
   * @brief Sets a 3 component vector uniform. Uses glUniform3fv()
   * @param name The name of the uniform.
   * @param value A pointer to the values to be set.
   */
  inline void setVec3(const char* name, const GLfloat* value) const {
    glUniform3fv(glGetUniformLocation(id_, name), 1, value);
  }

  /**
   * @brief Sets a 3 component vector uniform. Uses glUniform3f()
   * @param name The name of the uniform.
   * @param x value.
   * @param y value.
   * @param z value.
   */
  inline void setVec3(const char* name, GLfloat x, GLfloat y, GLfloat z) const {
    glUniform3f(glGetUniformLocation(id_, name), x, y, z);
  }

  /**
   * @brief Sets a 4 component vector uniform. Uses glUniform4fv()
   * @param name The name of the uniform.
   * @param value A pointer to the values to be set.
   */
  inline void setVec4(const char* name, const GLfloat* value) const {
    glUniform4fv(glGetUniformLocation(id_, name), 1, value);
  }

  /**
   * @brief Sets a 4 component vector uniform. Uses glUniform4f()
   * @param name The name of the uniform.
   * @param x value.
   * @param y value.
   * @param z value.
   * @param w value.
   */
  inline void setVec4(const char* name, GLfloat x, GLfloat y, GLfloat z, GLfloat w) const {
    glUniform4f(glGetUniformLocation(id_, name), x, y, z, w);
  }

  /**
   * @brief Activates the shader.
   */
  inline void use() const { glUseProgram(id_); }

 private:

  GLuint id_ {};
};

/**
 * @brief A RAII vertex buffer object wrapper.
 */
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

  /**
   * @brief Binds the VBO.
   */
  void bind() const { glBindBuffer(GL_ARRAY_BUFFER, id_); }

  /**
   * @brief Sets up the data for the buffer.
   * @param vertices A std::vector of floats to use as data.
   */
  void setup(const GLfloatVector& vertices, GLenum usage = GL_STATIC_DRAW);

  /**
   * @brief Sets up the data for the buffer.
   * @param vertices A pointer to an array of floats to use as data.
   * @param size The size in bytes of the data.
   */
  void setup(const GLfloat* vertices, GLsizeiptr size, GLenum usage = GL_STATIC_DRAW);

  /**
   * @brief Unbinds the VBO.
   */
  void unbind() const { glBindBuffer(GL_ARRAY_BUFFER, 0); }

 private:

  GLuint id_ {};
};

/**
 * @brief A RAII element buffer object wrapper.
 */
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

  /**
   * @brief Generates an EBO by removing duplicate coordinates and creating an index array.
   * @param vertices Vector of vertices.
   * @param indices Vector of indices.
   */
  static void generateEBO(GLfloatVector& vertices, GLuintVector& indices);

  /**
   * @brief Binds the EBO.
   */
  void bind() const { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id_); }

  /**
   * @brief Sets up the data for the buffer.
   * @param vertices A std::vector of uints to use as data.
   */
  void setup(const GLuintVector& indices, GLenum usage = GL_STATIC_DRAW);

  /**
   * @brief Sets up the data for the buffer.
   * @param indices A pointer to an array of unsigned ints to use as data.
   * @param size The size in bytes of the data.
   */
  void setup(const GLuint* indices, GLsizeiptr size, GLenum usage = GL_STATIC_DRAW);

  /**
   * @brief Unbinds the EBO.
   */
  void unbind() const { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); }

 private:

  GLuint id_ {};
};

/**
 * @brief A RAII vertex array object wrapper.
 */
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

  /**
   * @brief Binds the VAO.
   */
  void bind() const { glBindVertexArray(id_); }

  /**
   * @brief Specifies how OpenGL should interpret the vertex buffer data whenever a draw call is made.
   * @param vbo The vertex buffer object to be binded.
   * @param layout Specifies the index of the generic vertex attribute to be modified. Must match the layout in the shader.
   * @param components Specifies the number of components per generic vertex attribute. Must be 1, 2, 3, 4.
   * @param type Type of the data.
   * @param stride Specifies the byte offset between consecutive generic vertex attributes.
   * @param offset Specifies a offset of the first component of the first generic vertex attribute in the array in the data store.
   * @param normalize Specifies whether fixed-point data values should be normalized.
   */
  void linkAttrib(const VBO& vbo, GLuint layout, GLuint components, GLenum type, GLsizeiptr stride, void* offset, GLboolean normalize = GL_FALSE) const;

  /**
   * @brief Unbinds the VAO.
   */
  void unbind() const { glBindVertexArray(0); }

 private:

  GLuint id_ {};
};

/**
 * @brief A texture wrapper.
 */
class Texture2D {

 public:

  Texture2D() = default;
  Texture2D(GLuint id): id_(id) {}

  /**
   * @brief Bind the texture.
   */
  void bind() const { glBindTexture(GL_TEXTURE_2D, id_); }

  /**
   * @brief Unbinds the texture.
   */
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
