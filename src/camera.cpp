#include "include/camera.hpp"

ktp::Camera::Camera(const glm::vec3& position, const glm::vec3& up, float yaw, float pitch) {
  position_ = position;
  world_up_ = up;
  yaw_ = yaw;
  pitch_ = pitch;
  updateCameraVectors();
}

ktp::Camera::Camera(float pos_x, float pos_y, float pos_z, float up_x, float up_y, float up_z, float yaw, float pitch) {
  position_ = glm::vec3(pos_x, pos_y, pos_z);
  world_up_ = glm::vec3(up_x, up_y, up_z);
  yaw_ = yaw;
  pitch_ = pitch;
  updateCameraVectors();
}

void ktp::Camera::keyboardMovement(CameraMovement direction, float delta_time) {
  const auto velocity {movement_speed_ * delta_time};
  if (direction == CameraMovement::Forward)  position_ += front_ * velocity;
  if (direction == CameraMovement::Backward) position_ -= front_ * velocity;
  if (direction == CameraMovement::Left)     position_ -= right_ * velocity;
  if (direction == CameraMovement::Right)    position_ += right_ * velocity;
}

void ktp::Camera::mouseMovement(float x_offset, float y_offset, bool constrain_pitch) {
  yaw_   += x_offset * mouse_sensitivity_;
  pitch_ -= y_offset * mouse_sensitivity_;
  // Make sure that when pitch is out of bounds, screen doesn't get flipped.
  if (constrain_pitch) {
    if (pitch_ >  89.f) pitch_ =  89.f;
    if (pitch_ < -89.f) pitch_ = -89.f;
  }
  updateCameraVectors();
}

void ktp::Camera::mouseScroll(float y_offset) {
  zoom_ -= y_offset;
  if (zoom_ < 1.f) zoom_ = 1.f;
  if (zoom_ > 45.f) zoom_ = 45.f;
}

void ktp::Camera::updateCameraVectors() {
  // Calculate the new front_ vector
  const glm::vec3 new_front {
    glm::cos(glm::radians(yaw_)) * glm::cos(glm::radians(pitch_)),  // x
    glm::sin(glm::radians(pitch_)),                                 // y
    glm::sin(glm::radians(yaw_)) * glm::cos(glm::radians(pitch_))   // z
  };
  front_ = glm::normalize(new_front);
  // Also re-calculate the right_ and up_ vectors.
  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
  right_ = glm::normalize(glm::cross(front_, world_up_));
  up_    = glm::normalize(glm::cross(right_, front_));
}
