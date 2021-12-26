#include "sdl2_log.hpp"
#include "sdl2_opengl.hpp"
#include "sdl2_window.hpp"
#include <array>
#include <fstream>
#include <sstream>

/* SDL2_GL */

GLenum ktp::SDL2_GL::glCheckError_(const char* file, int line) {
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

std::vector<GLfloat> ktp::SDL2_GL::cube(GLfloat size) {
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

bool ktp::SDL2_GL::initGLEW(SDL2_GLContext& context, const SDL2_Window& window) {
  context.context() = SDL_GL_CreateContext(window.getWindow());
  if (!context.context()) {
    logSDL2Error("SDL_GL_CreateContext", SDL_LOG_CATEGORY_RENDER);
    return false;
  } else {
    glEnable(GL_MULTISAMPLE);
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

/* VBO */

ktp::VBO::VBO() {
  glGenBuffers(1, &id_);
  glCheckError();
}

void ktp::VBO::setup(const GLfloatVector& vertices) {
  glBindBuffer(GL_ARRAY_BUFFER, id_);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);
  glCheckError();
}

void ktp::VBO::setup(const GLfloat* vertices, GLsizeiptr size) {
  glBindBuffer(GL_ARRAY_BUFFER, id_);
  glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
  glCheckError();
}

/* EBO */

ktp::EBO::EBO() {
  glGenBuffers(1, &id_);
  glCheckError();
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

void ktp::EBO::setup(const GLuintVector& indices) {
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id_);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);
  glCheckError();
}

void ktp::EBO::setup(const GLuint* indices, GLsizeiptr size) {
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id_);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, indices, GL_STATIC_DRAW);
  glCheckError();
}

/* VAO */

ktp::VAO::VAO() {
  glGenVertexArrays(1, &id_);
  glCheckError();
}

void ktp::VAO::linkAttrib(const VBO& vbo, GLuint layout, GLuint components, GLenum type, GLsizeiptr stride, void* offset) const {
  glBindVertexArray(id_);
  vbo.bind();
  glVertexAttribPointer(
    layout,       // index: specifies the index of the generic vertex attribute to be modified. Must match the layout in the shader
    components,   // size: specifies the number of components per generic vertex attribute. Must be 1, 2, 3, 4.
    type,         // type of the data
    GL_FALSE,     // normalized: specifies whether fixed-point data values should be normalized
    stride,       // stride: specifies the byte offset between consecutive generic vertex attributes
    offset        // pointer: specifies a offset of the first component of the first generic vertex attribute in the array in the data store
  );
  glEnableVertexAttribArray(layout);
  glCheckError();
  vbo.unbind();
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
