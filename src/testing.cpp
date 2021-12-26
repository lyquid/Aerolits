#include "include/aerolite.hpp"
#include "include/box2d_utils.hpp"
#include "include/palette.hpp"
#include "include/random.hpp"
#include "include/resources.hpp"
#include "include/testing.hpp"
#include "sdl2_wrappers/sdl2_geometry.hpp"
#include "sdl2_wrappers/sdl2_log.hpp"
#include "sdl2_wrappers/sdl2_timer.hpp"

// ktp::Testing::Testing() {
//   logMessage("testing ctt");
// }

void ktp::Testing::draw() const {
  shader_program_.setMat4f("mvp", glm::value_ptr(mvp_));
  texture_.bind();
  vao_.bind();
  glDrawArrays(GL_TRIANGLES, 0, vertices_.size() / 3);
}

void ktp::Testing::init() {
  texture_ = Resources::getTexture("title");
  shader_program_ = Resources::getShader("testing");

  vertices_ = {
        // first triangle
     0.5f,  0.5f, 0.0f,  // top right
     0.5f, -0.5f, 0.0f,  // bottom right
    -0.5f,  0.5f, 0.0f,  // top left
    // second triangle
     0.5f, -0.5f, 0.0f,  // bottom right
    -0.5f, -0.5f, 0.0f,  // bottom left
    -0.5f,  0.5f, 0.0f   // top left
  };
  const GLfloatVector texture_coords {
      1.0f, 0.0f,   // top right
       1.0f, 1.0f,   // bottom right
       0.0f, 0.0f,    // top left

       1.0f, 1.0f,   // bottom right
       0.0f, 1.0f,   // bottom left
       0.0f, 0.0f    // top left
  };
  //const GLfloatVector texture_coords {AerolitePhysicsComponent::convertToUV(vertices_)};
  uv_.setup(texture_coords);

  vbo_.setup(vertices_);
  vao_.linkAttrib(vbo_, 0, 3, GL_FLOAT, 0, nullptr);
  vao_.linkAttrib(uv_, 1, 2, GL_FLOAT, 0, nullptr);
}

void ktp::Testing::update(float delta_time) {
  // const auto state {SDL_GetKeyboardState(nullptr)};
  // const float kSpeed {1.5f * delta_time};
  // if (state[SDL_SCANCODE_W]) {
  //   camera_pos_ += kSpeed * camera_front_;
  // }
  // if (state[SDL_SCANCODE_S]) {
  //   camera_pos_ -= kSpeed * camera_front_;
  // }
  // if (state[SDL_SCANCODE_A]) {
  //   camera_pos_ -= glm::normalize(glm::cross(camera_front_, camera_up_)) * kSpeed;
  // }
  // if (state[SDL_SCANCODE_D]) {
  //   camera_pos_ += glm::normalize(glm::cross(camera_front_, camera_up_)) * kSpeed;
  // }

  // auto yaw {-90.f};
  // auto pitch {0.f};
  // glm::vec3 direction {};
  // direction.x = SDL_cosf(glm::radians(yaw)) * SDL_cosf(glm::radians(pitch));
  // direction.y = SDL_sinf(glm::radians(pitch));
  // direction.z = SDL_sinf(glm::radians(yaw)) * SDL_cosf(glm::radians(pitch));

  // const auto step {0.5f * delta_time};
  // for (auto i = 0; i < vertices_color_.size(); ++i) {
  //   if (vertices_color_direction_[i]) {
  //     if (vertices_color_[i] < 1) {
  //       vertices_color_[i] += step * generateRand(0.f, 1.f);
  //     } else {
  //       vertices_color_[i] -= step * generateRand(0.f, 1.f);
  //       vertices_color_direction_[i] = !vertices_color_direction_[i];
  //     }
  //   } else {
  //     if (vertices_color_[i] > 0) {
  //       vertices_color_[i] -= step * generateRand(0.f, 1.f);
  //     } else {
  //       vertices_color_[i] += step * generateRand(0.f, 1.f);
  //       vertices_color_direction_[i] = !vertices_color_direction_[i];
  //     }
  //   }
  // }
  //cube_.colors_.bind();
  // SDL2_GL::glBufferDataFromVector(GL_ARRAY_BUFFER, color_vertices_, GL_STATIC_DRAW);

  glm::mat4 model {1.f};
  model = glm::rotate(model, glm::radians(angle_ * delta_time), glm::vec3(0.f, 1.f, 0.f));
  view_ = glm::lookAt(camera_pos_, camera_pos_ + camera_front_, camera_up_);
  mvp_ = projection_ * view_ * model;
}
