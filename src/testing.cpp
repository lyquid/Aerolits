#include "include/paths.hpp"
#include "include/testing.hpp"
#include "sdl2_wrappers/sdl2_geometry.hpp"
#include "sdl2_wrappers/sdl2_log.hpp"
#include <cstdio>

ktp::Testing::Testing() {}

void ktp::Testing::draw_OLD() const {
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

void ktp::Testing::draw() const {
  // 1st attribute buffer : vertices
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_);
  glVertexAttribPointer(
    0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
    3,                  // size
    GL_FLOAT,           // type
    GL_FALSE,           // normalized?
    0,                  // stride
    (void*)0            // array buffer offset
  );
  // Use our shader
  glUseProgram(program_id_);
  // Send our transformation to the currently bound shader, in the "MVP" uniform
  glUniformMatrix4fv(matrix_id_, 1, GL_FALSE, &mvp_[0][0]);
  // Draw the triangle !
  glDrawArrays(GL_TRIANGLES, 0, 3); // Starting from vertex 0; 3 vertices total -> 1 triangle
  glDisableVertexAttribArray(0);
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

void ktp::Testing::tutorial() {
  // Modify ModelMatrix to translate, rotate, then scale the triangle
  //model_ = glm::translate(model_, glm::vec3(0.f, 0.0f, 0.0f));
  //model_ = glm::rotate(model_, glm::radians(60.0f), glm::vec3(1.f, 1.f, 1.f));
  //model_ = glm::scale(model_, glm::vec3(1.f, 1.f, 1.f));

  // Projection matrix : Field of View, aspect ratio, display range : 0.1 unit <-> 100 units
  projection_ = glm::perspective(glm::radians(45.0f), 16.f / 9.f, 0.1f, 100.0f);
  // Ortho
  //projection_ = glm::ortho(-2.f, 2.f, -1.5f, 1.5f, 0.1f, 100.f);
  
  // Camera matrix
  view_ = glm::lookAt(
    glm::vec3 {0, 0, 3}, // Camera is at (4,3,3), in World Space
    glm::vec3 {0, 0, 0}, // and looks at the origin
    glm::vec3 {0, 1, 0}  // Head is up (set to 0,-1,0 to look upside-down)
  );
  // Our ModelViewProjection : multiplication of our 3 matrices
  mvp_ = projection_ * view_ * model_;

  glGenVertexArrays(1, &vertex_array_id_);
  glBindVertexArray(vertex_array_id_);

  constexpr GLfloat vertex_buffer_data[] = {
    -1.0f, -1.0f, 0.0f,
     1.0f, -1.0f, 0.0f,
     0.0f,  1.0f, 0.0f,
  };

  // Generate 1 buffer, put the resulting identifier in vertex_buffer_
  glGenBuffers(1, &vertex_buffer_);
  // The following commands will talk about our 'vertexbuffer' buffer
  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_);
  // Give our vertices to OpenGL.
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_buffer_data), vertex_buffer_data, GL_STATIC_DRAW);

  const auto path {getResourcesPath("shaders")};
  program_id_ = SDL2_GLEW::loadShaders(path + "simple_vertex_shader.glsl", path + "simple_fragment_shader.glsl");
  matrix_id_ = glGetUniformLocation(program_id_, "MVP");
}
