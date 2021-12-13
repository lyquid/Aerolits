#include "sdl2_log.hpp"
#include "sdl2_opengl.hpp"
#include "sdl2_window.hpp"
#include <fstream>
#include <sstream>

/* SDL2_GL */

GLenum ktp::SDL2_GL::glCheckError_(const char* file, int line) {
  GLenum error_code;
  while ((error_code = glGetError()) != GL_NO_ERROR) {
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

bool ktp::SDL2_GL::initGLEW(SDL_GLContext& context, SDL2_Window& window) {
  context = SDL_GL_CreateContext(window.getWindow());
  if (!context) {
    logSDL2Error("SDL_GL_CreateContext", SDL_LOG_CATEGORY_RENDER);
    return false;
  } else {
    glEnable(GL_MULTISAMPLE);
    // Enable depth test
    // glEnable(GL_DEPTH_TEST);
    // Accept fragment if it closer to the camera than the former one
    // glDepthFunc(GL_LESS);
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
    // glViewport(0, 0, 800, 600);
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

/* SHADER PROGRAM */

ktp::ShaderProgram::ShaderProgram(const std::string& vertex_shader_path, const std::string& fragment_shader_path) {
  setup(vertex_shader_path, fragment_shader_path);
}

void ktp::ShaderProgram::printProgramLog(GLuint program) {
  // Make sure name is program
  if (glIsProgram(program)) {
    // Program log length
    int info_log_length{}, max_length{};
    // Get info string length
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &max_length);
    std::string info_log {};
    info_log.resize(max_length);
    // Get info log
    glGetProgramInfoLog(program, max_length, &info_log_length, info_log.data());
    if (info_log_length > 0) logMessage(info_log);
  } else {
    logError("printProgramLog(): Name " + std::to_string(program) + " is not a program.");
  }
}

void ktp::ShaderProgram::printShaderLog(GLuint shader) {
  // Make sure name is shader
  if (glIsShader(shader)) {
    // Shader log length
    int info_log_length {}, max_length {};
    // Get info string length
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &max_length);
    std::string info_log {};
    info_log.resize(max_length);
    // Get info log
    glGetShaderInfoLog(shader, max_length, &info_log_length, info_log.data());
    if (info_log_length > 0) logMessage(info_log);
  } else {
    logError("printShaderLog(): Name " + std::to_string(shader) + " is not a shader.");
  }
}

void ktp::ShaderProgram::setup(const std::string& vertex_shader_path, const std::string& fragment_shader_path) {
  // Create the shaders
	GLuint vertex_shader_id {glCreateShader(GL_VERTEX_SHADER)};
	GLuint fragment_shader_id {glCreateShader(GL_FRAGMENT_SHADER)};
  // Read the Vertex Shader code from the file
	std::string vertex_shader_code {};
	std::ifstream vertex_shader_stream {vertex_shader_path, std::ios::in};
	if (vertex_shader_stream.is_open()) {
		std::stringstream sstr {};
		sstr << vertex_shader_stream.rdbuf();
		vertex_shader_code = sstr.str();
		vertex_shader_stream.close();
	} else {
    logError("Could NOT open vertex shader file", vertex_shader_path);
	}
  // Read the Fragment Shader code from the file
	std::string fragment_shader_code {};
	std::ifstream fragment_shader_stream {fragment_shader_path, std::ios::in};
	if (fragment_shader_stream.is_open()) {
		std::stringstream sstr {};
		sstr << fragment_shader_stream.rdbuf();
		fragment_shader_code = sstr.str();
		fragment_shader_stream.close();
	} else {
    logError("Could NOT open fragment shader file", fragment_shader_path);
	}
  // Compile Vertex Shader
  logMessage("Compiling vertex shader " + vertex_shader_path);
	const auto vertex_source_pointer {vertex_shader_code.c_str()};
	glShaderSource(vertex_shader_id, 1, &vertex_source_pointer, nullptr);
	glCompileShader(vertex_shader_id);
  glCheckError();
  printShaderLog(vertex_shader_id);
  // Compile Fragment Shader
  logMessage("Compiling fragment shader " + fragment_shader_path);
	const auto fragment_source_pointer {fragment_shader_code.c_str()};
	glShaderSource(fragment_shader_id, 1, &fragment_source_pointer, nullptr);
	glCompileShader(fragment_shader_id);
  glCheckError();
  printShaderLog(fragment_shader_id);
  // Link the program
	id_ = glCreateProgram();
  glCheckError();
	glAttachShader(id_, vertex_shader_id);
	glAttachShader(id_, fragment_shader_id);
	glLinkProgram(id_);
  printProgramLog(id_);
  glCheckError();
  // clean
	// glDetachShader(id_, vertex_shader_id);
	// glDetachShader(id_, fragment_shader_id);
	glDeleteShader(vertex_shader_id);
	glDeleteShader(fragment_shader_id);
  glCheckError();
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

void ktp::VBO::setup(GLfloat* vertices, GLsizeiptr size) {
  glBindBuffer(GL_ARRAY_BUFFER, id_);
  glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
  glCheckError();
}

/* EBO */

ktp::EBO::EBO() {
  glGenBuffers(1, &id_);
  glCheckError();
}

void ktp::EBO::setup(const GLuintVector& indices) {
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id_);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);
  glCheckError();
}

void ktp::EBO::setup(GLuint* indices, GLsizeiptr size) {
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
