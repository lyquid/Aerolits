#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace ktp {

enum class CameraMovement {
  Forward,
  Backward,
  Left,
  Right,
  count
};

/**
 * @brief An abstract camera class that processes input and calculates the corresponding Euler angles,
 *        vectors and matrices for use in OpenGL.
 */
class Camera {
 public:

  Camera(const glm::vec3& position = glm::vec3{0.f, 0.f, 0.f}, const glm::vec3& up = glm::vec3{0.f, 1.f, 0.f}, float yaw = -90.f, float pitch = 0.f);
  Camera (float pos_x, float pos_y, float pos_z, float up_x, float up_y, float up_z, float yaw, float pitch);

  /**
   * @brief Processes keyboard input to move the camera accordingly.
   * @param direction Where should the camera go.
   * @param delta_time The delta time!
   */
  void keyboardMovement(CameraMovement direction, float delta_time);

  /**
   * @brief Processes mouse movement to move the camera accordingly.
   * @param x_offset
   * @param y_offset
   * @param constrain_pitch To make sure that when pitch is out of bounds, screen doesn't get flipped.
   */
  void mouseMovement(float x_offset, float y_offset, bool constrain_pitch = true);

  /**
   * @brief Sets the zoom based on the mousewheel spinning.
   * @param y_offset The amount spinned.
   */
  void mouseScroll(float y_offset);

  /**
   * @brief Calculates the view matrix using Euler angles and the glm::lookAt() matrix.
   * @return The view matrix.
   */
  inline auto viewMatrix() const { return glm::lookAt(position_, position_ + front_, up_); }

  /**
   * @brief Get the zoom!
   * @return The zoom's current value.
   */
  inline auto zoom() const { return zoom_; }

 private:

  /**
   * @brief Calculates the front vector from the camera's (updated) Euler angles.
   */
  void updateCameraVectors();

  // camera attributes
  glm::vec3 position_ {0.f, 0.f, 0.f};
  glm::vec3 front_ {0.f, 0.f, -1.f};
  glm::vec3 up_ {};
  glm::vec3 right_ {};
  glm::vec3 world_up_ {0.f, 1.f, 0.f};
  // Euler angles
  float yaw_ {-90.f};
  float pitch_ {};
  // camera options
  float movement_speed_ {2.5f};
  float mouse_sensitivity_ {0.1f};
  float zoom_ {45.f};
};

} // namespace ktp
