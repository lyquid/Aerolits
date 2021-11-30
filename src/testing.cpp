#include "include/paths.hpp"
#include "include/random.hpp"
#include "include/testing.hpp"
#include "sdl2_wrappers/sdl2_geometry.hpp"
#include "sdl2_wrappers/sdl2_log.hpp"
#include <cstdio>

ktp::Testing::Testing() {}

void ktp::Testing::draw() const {
  // TRIANGLE
  // glEnableVertexAttribArray(0);
  // glBindBuffer(GL_ARRAY_BUFFER, tri_vertex_buffer_);
  // glVertexAttribPointer(
  //   0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
  //   3,                  // size
  //   GL_FLOAT,           // type
  //   GL_FALSE,           // normalized?
  //   0,                  // stride
  //   (void*)0            // array buffer offset
  // );
  // // Draw the triangle !
  // glDrawArrays(GL_TRIANGLES, 0, 3); // Starting from vertex 0; 3 vertices total -> 1 triangle
  // glDisableVertexAttribArray(0);

  // Use our shader
  glUseProgram(program_.id());
  // Send our transformation to the currently bound shader, in the "MVP" uniform
  glUniformMatrix4fv(matrix_id_, 1, GL_FALSE, &mvp_[0][0]);
  // CUBE
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
  // 2nd attribute buffer : colors
  glEnableVertexAttribArray(1);
  glBindBuffer(GL_ARRAY_BUFFER, color_buffer_);
  glVertexAttribPointer(
    1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
    3,                                // size
    GL_FLOAT,                         // type
    GL_FALSE,                         // normalized?
    0,                                // stride
    (void*)0                          // array buffer offset
  );
  // Draw the cube !
  glDrawArrays(GL_TRIANGLES, 0, 12 * 3); // 12*3 indices starting at 0 -> 12 triangles -> 6 squares

  glDisableVertexAttribArray(0);
  glDisableVertexAttribArray(1);
}

void ktp::Testing::tutorial() {
  // TRIANGLE
  // glGenVertexArrays(1, &tri_vertex_array_id_);
  // glBindVertexArray(tri_vertex_array_id_);
  // glGenBuffers(1, &tri_vertex_buffer_);
  // glBindBuffer(GL_ARRAY_BUFFER, tri_vertex_buffer_);
  // SDL2_GLEW::glBufferDataFromVector(GL_ARRAY_BUFFER, tri_vertex_buffer_data_, GL_STATIC_DRAW);



  // CUBE
  glGenVertexArrays(1, &vertex_array_id_);
  glBindVertexArray(vertex_array_id_);
  // Create and compile our GLSL program from the shaders
  const auto path {getResourcesPath("shaders")};
  program_ = SDL2_GLEW::loadShaders(path + "simple_vertex_shader.glsl", path + "simple_fragment_shader.glsl");
  // Get a handle for our "MVP" uniform
  matrix_id_ = glGetUniformLocation(program_.id(), "MVP");
  // Projection matrix : Field of View, aspect ratio, display range : 0.1 unit <-> 100 units
  projection_ = glm::perspective(glm::radians(45.0f), 16.f / 9.f, 0.1f, 100.0f);
  //projection_ = glm::ortho(-2.f, 2.f, -1.5f, 1.5f, 0.1f, 100.f); // ortho
  // Camera matrix
  view_ = glm::lookAt(
    glm::vec3 {4, 3, 3}, // Camera is at (4,3,3), in World Space
    glm::vec3 {0, 0, 0}, // and looks at the origin
    glm::vec3 {0, 1, 0}  // Head is up (set to 0,-1,0 to look upside-down)
  );
  // Our ModelViewProjection: multiplication of our 3 matrices
  mvp_ = projection_ * view_ * model_;
  // Generate 1 buffer, put the resulting identifier in vertex_buffer_
  glGenBuffers(1, &vertex_buffer_);
  // The following commands will talk about our 'vertexbuffer' buffer
  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_);
  // Give our vertices to OpenGL.
  SDL2_GLEW::glBufferDataFromVector(GL_ARRAY_BUFFER, vertex_buffer_data_, GL_STATIC_DRAW);

  // CUBE COLORS
  for (std::size_t i = 0; i < vertex_buffer_data_.size(); ++i) {
    color_buffer_data_.push_back(generateRand(0.f, 1.f));
    color_buffer_data_pattern_.push_back(generateRand(0u, 1u));
  }
  glGenBuffers(1, &color_buffer_);
  glBindBuffer(GL_ARRAY_BUFFER, color_buffer_);
  SDL2_GLEW::glBufferDataFromVector(GL_ARRAY_BUFFER, color_buffer_data_, GL_STATIC_DRAW);
}

void ktp::Testing::update(float delta_time) {
  for (std::size_t i = 0; i < vertex_buffer_data_.size(); ++i) {
    if (color_buffer_data_pattern_[i]) {
      if (color_buffer_data_[i] < 1.f) {
        color_buffer_data_[i] += generateRand(0.0001f, 0.01f);
      } else {
        color_buffer_data_pattern_[i] = !color_buffer_data_pattern_[i];
      }
    } else {
      if (color_buffer_data_[i] > 0.f) {
        color_buffer_data_[i] -= generateRand(0.0001f, 0.01f);;
      } else {
        color_buffer_data_pattern_[i] = !color_buffer_data_pattern_[i];
      }
    }
  }
  SDL2_GLEW::glBufferDataFromVector(GL_ARRAY_BUFFER, color_buffer_data_, GL_STATIC_DRAW);
}
