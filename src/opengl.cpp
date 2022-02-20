#include "include/opengl.hpp"
#include "sdl2_wrappers/sdl2_log.hpp"

/* SDL2_GL */

GLenum ktp::glCheckError_(const char* file, int line) {
  GLenum error_code {};
  while (error_code = glGetError()) {
    std::string error_msg {};
    switch (error_code) {
      case GL_INVALID_ENUM:                  error_msg = "INVALID_ENUM"; break;
      case GL_INVALID_VALUE:                 error_msg = "INVALID_VALUE"; break;
      case GL_INVALID_OPERATION:             error_msg = "INVALID_OPERATION"; break;
      case GL_STACK_OVERFLOW:                error_msg = "STACK_OVERFLOW"; break;
      case GL_STACK_UNDERFLOW:               error_msg = "STACK_UNDERFLOW"; break;
      case GL_OUT_OF_MEMORY:                 error_msg = "OUT_OF_MEMORY"; break;
      case GL_INVALID_FRAMEBUFFER_OPERATION: error_msg = "INVALID_FRAMEBUFFER_OPERATION"; break;
    }
    error_msg = error_msg + " in file " + file + " (" + std::to_string(line) + ')';
    logError(error_msg, SDL_LOG_CATEGORY_RENDER);
  }
  return error_code;
}

std::vector<GLfloat> ktp::cube(GLfloat size) {
  const auto good_size {SDL_fabsf(size)};
  std::vector<GLfloat> vertices {
    -1.f * good_size, -1.f * good_size, -1.f * good_size, // triangle 1 : begin
    -1.f * good_size, -1.f * good_size,  1.f * good_size,
    -1.f * good_size,  1.f * good_size,  1.f * good_size, // triangle 1 : end
     1.f * good_size,  1.f * good_size, -1.f * good_size, // triangle 2 : begin
    -1.f * good_size, -1.f * good_size, -1.f * good_size,
    -1.f * good_size,  1.f * good_size, -1.f * good_size, // triangle 2 : end
     1.f * good_size, -1.f * good_size,  1.f * good_size,
    -1.f * good_size, -1.f * good_size, -1.f * good_size,
     1.f * good_size, -1.f * good_size, -1.f * good_size,
     1.f * good_size,  1.f * good_size, -1.f * good_size,
     1.f * good_size, -1.f * good_size, -1.f * good_size,
    -1.f * good_size, -1.f * good_size, -1.f * good_size,
    -1.f * good_size, -1.f * good_size, -1.f * good_size,
    -1.f * good_size,  1.f * good_size,  1.f * good_size,
    -1.f * good_size,  1.f * good_size, -1.f * good_size,
     1.f * good_size, -1.f * good_size,  1.f * good_size,
    -1.f * good_size, -1.f * good_size,  1.f * good_size,
    -1.f * good_size, -1.f * good_size, -1.f * good_size,
    -1.f * good_size,  1.f * good_size,  1.f * good_size,
    -1.f * good_size, -1.f * good_size,  1.f * good_size,
     1.f * good_size, -1.f * good_size,  1.f * good_size,
     1.f * good_size,  1.f * good_size,  1.f * good_size,
     1.f * good_size, -1.f * good_size, -1.f * good_size,
     1.f * good_size,  1.f * good_size, -1.f * good_size,
     1.f * good_size, -1.f * good_size, -1.f * good_size,
     1.f * good_size,  1.f * good_size,  1.f * good_size,
     1.f * good_size, -1.f * good_size,  1.f * good_size,
     1.f * good_size,  1.f * good_size,  1.f * good_size,
     1.f * good_size,  1.f * good_size, -1.f * good_size,
    -1.f * good_size,  1.f * good_size, -1.f * good_size,
     1.f * good_size,  1.f * good_size,  1.f * good_size,
    -1.f * good_size,  1.f * good_size, -1.f * good_size,
    -1.f * good_size,  1.f * good_size,  1.f * good_size,
     1.f * good_size,  1.f * good_size,  1.f * good_size,
    -1.f * good_size,  1.f * good_size,  1.f * good_size,
     1.f * good_size, -1.f * good_size,  1.f * good_size
  };
  return vertices;
}

/* VBO */

ktp::VBO::VBO() {
  glGenBuffers(1, &id_);
}

// this setup is needed when you pass nullptr for a later use with subData
void ktp::VBO::setup(const GLfloat* vertices, GLsizeiptr size, GLenum usage) {
  glBindBuffer(GL_ARRAY_BUFFER, id_);
  glBufferData(GL_ARRAY_BUFFER, size, vertices, usage);
}

/* EBO */

ktp::EBO::EBO() {
  glGenBuffers(1, &id_);
}

void ktp::EBO::generateEBO(GLfloatVector& vertices, GLuintVector& indices) {
  GLfloatVector unique_coords {};
  indices.clear();
  // push the first element
  indices.push_back(0);
  // push the first three coords
  unique_coords.push_back(vertices[0]);
  unique_coords.push_back(vertices[1]);
  unique_coords.push_back(vertices[2]);
  // find repeated coords by groups of 3
  for (std::size_t i = 3; i < vertices.size(); i += 3) {
    bool found {false};
    for (std::size_t j = 0; j < unique_coords.size(); j += 3) {
      if (vertices[i] == unique_coords[j] && vertices[i + 1] == unique_coords[j + 1] && vertices[i + 2] == unique_coords[j + 2]) {
        // coord already in the list
        found = true;
        // so, new index will point to...
        indices.push_back(j / 3u);
        break;
      }
    }
    if (!found) {
      // new unique coord
      unique_coords.push_back(vertices[i]);
      unique_coords.push_back(vertices[i + 1]);
      unique_coords.push_back(vertices[i + 2]);
      // the new index points at...
      indices.push_back((unique_coords.size() - 1u) / 3u);
    }
  }
  // the new vertices
  vertices = std::move(unique_coords);
}

void ktp::EBO::setup(const GLuintVector& indices, GLenum usage) {
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id_);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), usage);
}

void ktp::EBO::setup(const GLuint* indices, GLsizeiptr size, GLenum usage) {
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id_);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, indices, usage);
}

/* VAO */

ktp::VAO::VAO() {
  glGenVertexArrays(1, &id_);
}

void ktp::VAO::linkAttrib(const VBO& vbo, GLuint layout, GLuint components, GLenum type, GLsizeiptr stride, void* offset, GLboolean normalize) const {
  glBindVertexArray(id_);
  vbo.bind();
  glEnableVertexAttribArray(layout);
  glVertexAttribPointer(
    layout,       // index: specifies the index of the generic vertex attribute to be modified. Must match the layout in the shader
    components,   // size: specifies the number of components per generic vertex attribute. Must be 1, 2, 3, 4.
    type,         // type of the data
    normalize,    // normalize: specifies whether fixed-point data values should be normalized
    stride,       // stride: specifies the byte offset between consecutive generic vertex attributes
    offset        // pointer: specifies a offset of the first component of the first generic vertex attribute in the array in the data store
  );
}

void ktp::VAO::linkAttribFast(GLuint layout, GLuint components, GLenum type, GLsizeiptr stride, void* offset, GLboolean normalize) const {
  glEnableVertexAttribArray(layout);
  glVertexAttribPointer(
    layout,       // index: specifies the index of the generic vertex attribute to be modified. Must match the layout in the shader
    components,   // size: specifies the number of components per generic vertex attribute. Must be 1, 2, 3, 4.
    type,         // type of the data
    normalize,    // normalize: specifies whether fixed-point data values should be normalized
    stride,       // stride: specifies the byte offset between consecutive generic vertex attributes
    offset        // pointer: specifies a offset of the first component of the first generic vertex attribute in the array in the data store
  );
}

/* Texture2D */

// void ktp::Texture2D::generate(GLsizei width, GLsizei height, unsigned char* data) {
//   width_ = width;
//   height_ = height;
//   // create Texture
//   glBindTexture(GL_TEXTURE_2D, id_);
//   glTexImage2D(GL_TEXTURE_2D, 0, internal_format_, width, height, 0, image_format_, GL_UNSIGNED_BYTE, data);
//   // set Texture wrap and filter modes
//   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap_s_);
//   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap_t_);
//   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter_min_);
//   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter_max_);
//   // unbind texture
//   glBindTexture(GL_TEXTURE_2D, 0);
// }
