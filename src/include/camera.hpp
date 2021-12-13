#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace ktp {

class Camera {
 public:
  Camera() { updateCameraVectors(); }
  // const glm::mat4 projection_ {glm::perspective(glm::radians(45.f), 16.f / 9.f, 0.1f, 100.f)};
  // returns the view matrix calculated using Euler Angles and the LookAt Matrix
  auto viewMatrix() const {
    return glm::lookAt(position_, position_ + front_, up_);
  }
 private:
  // calculates the front vector from the Camera's (updated) Euler Angles
  void updateCameraVectors() {
    // calculate the new Front vector
    // glm::vec3 front;
    // front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    // front.y = sin(glm::radians(Pitch));
    // front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    // Front = glm::normalize(front);
    // also re-calculate the Right and Up vector
    right_ = glm::normalize(glm::cross(front_, world_up_));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
    up_    = glm::normalize(glm::cross(right_, front_));
  }
  // camera Attributes
  glm::vec3 position_ {0.0f, 0.0f, 0.0f};
  glm::vec3 front_ {0.0f, 0.0f, -1.0f};
  glm::vec3 up_ {};
  glm::vec3 right_ {};
  glm::vec3 world_up_ {0.0f, 1.0f, 0.0f};
  // euler Angles
  // float Yaw;
  // float Pitch;
  // camera options
  // float MovementSpeed;
  // float MouseSensitivity;
  // float Zoom;
};

} // namespace ktp
