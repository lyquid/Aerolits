#include "include/testing.hpp"
#include "sdl2_wrappers/sdl2_geometry.hpp"
#include "sdl2_wrappers/sdl2_log.hpp"
#include <cstdio>

ktp::Testing::Testing() {}

void ktp::Testing::draw() const {
  // Clear color buffer
  // glClear(GL_COLOR_BUFFER_BIT);
  // Render quad
  if (gRenderQuad_) {
    // Bind program
    glUseProgram(gProgramID_);
    // Enable vertex position
    glEnableVertexAttribArray(gVertexPos2DLocation_);
    // Set vertex data
    glBindBuffer(GL_ARRAY_BUFFER, gVBO_);
    glVertexAttribPointer(gVertexPos2DLocation_, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), nullptr);
    // Set index data and render
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gIBO_);
    glDrawElements(GL_TRIANGLE_FAN, 4, GL_UNSIGNED_INT, nullptr);
    // Disable vertex position
    glDisableVertexAttribArray(gVertexPos2DLocation_);
    // Unbind program
    glUseProgram(NULL);
  }
}

bool ktp::Testing::initGL() {
  bool success {true};
  // Generate program
  gProgramID_ = glCreateProgram();
  // Create vertex shader
  GLuint vertexShader {glCreateShader(GL_VERTEX_SHADER)};
  // Get vertex source
  const GLchar* vertexShaderSource[] = {
    "#version 140\nin vec2 LVertexPos2D; void main() { gl_Position = vec4( LVertexPos2D.x, LVertexPos2D.y, 0, 1 ); }"
  };
  // Set vertex source
  glShaderSource(vertexShader, 1, vertexShaderSource, NULL);
  // Compile vertex source
  glCompileShader(vertexShader);
  // Check vertex shader for errors
  GLint vShaderCompiled {GL_FALSE};
  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &vShaderCompiled);
  if (vShaderCompiled != GL_TRUE) {
    logMessage("Unable to compile vertex shader " + std::to_string(vertexShader) + "!");
    SDL2_GLEW::printShaderLog(vertexShader);
    success = false;
  } else {
    // Attach vertex shader to program
    glAttachShader(gProgramID_, vertexShader);
    // Create fragment shader
    GLuint fragmentShader {glCreateShader(GL_FRAGMENT_SHADER)};
    // Get fragment source
    const GLchar* fragmentShaderSource[] = {
      "#version 140\nout vec4 LFragment; void main() { LFragment = vec4( 1.0, 1.0, 1.0, 1.0 ); }"
    };
    // Set fragment source
    glShaderSource(fragmentShader, 1, fragmentShaderSource, NULL);
    // Compile fragment source
    glCompileShader(fragmentShader);
    // Check fragment shader for errors
    GLint fShaderCompiled {GL_FALSE};
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &fShaderCompiled);
    if (fShaderCompiled != GL_TRUE) {
      logMessage("Unable to compile fragment shader " + std::to_string(fragmentShader) + "!");
      SDL2_GLEW::printShaderLog(fragmentShader);
      success = false;
    } else {
      // Attach fragment shader to program
      glAttachShader(gProgramID_, fragmentShader);
      // Link program
      glLinkProgram(gProgramID_);
      // Check for errors
      GLint programSuccess {GL_TRUE};
      glGetProgramiv(gProgramID_, GL_LINK_STATUS, &programSuccess);
      if (programSuccess != GL_TRUE) {
        logMessage("Error linking program " + std::to_string(gProgramID_) + "!");
        SDL2_GLEW::printProgramLog(gProgramID_);
        success = false;
      } else {
        // Get vertex attribute location
        gVertexPos2DLocation_ = glGetAttribLocation(gProgramID_, "LVertexPos2D");
        if (gVertexPos2DLocation_ == -1) {
          logMessage("LVertexPos2D is not a valid glsl program variable!");
          success = false;
        } else {
          // Initialize clear color
          glClearColor(0.f, 0.f, 0.f, 1.f);
          // VBO data
          const GLfloat vertexData[] = {
            -0.5f, -0.5f,
             0.5f, -0.5f,
             0.5f,  0.5f,
            -0.5f,  0.5f
          };
          // IBO data
          const GLuint indexData[] = { 0, 1, 2, 3 };
          // Create VBO
          glGenBuffers(1, &gVBO_);
          glBindBuffer(GL_ARRAY_BUFFER, gVBO_);
          glBufferData(GL_ARRAY_BUFFER, 2 * 4 * sizeof(GLfloat), vertexData, GL_STATIC_DRAW);
          // Create IBO
          glGenBuffers(1, &gIBO_);
          glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gIBO_);
          glBufferData(GL_ELEMENT_ARRAY_BUFFER, 4 * sizeof(GLuint), indexData, GL_STATIC_DRAW);
        }
      }
    }
  }
  return success;
}
