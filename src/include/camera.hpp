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

enum class Projection {
  Orthographic,
  Perspective,
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
   * @brief Points the camera to directions.
   * @param x_offset
   * @param y_offset
   * @param constrain_pitch To make sure that when pitch is out of bounds, screen doesn't get flipped.
   */
  void look(float x_offset, float y_offset, bool constrain_pitch = true);

  /**
   * @brief Moves the camera.
   * @param direction Where should the camera go.
   * @param delta_time The delta time!
   */
  void move(CameraMovement direction, float delta_time);

  /**
   * @brief
   * @return auto
   */
  inline auto projectionMatrix() const { return projection_; }

  /**
   * @brief Sets the orthographic matrix projection.
   */
  inline void setOrthographicMatrix(const glm::mat4& ortho) { ortho_ = ortho; }

  /**
   * @brief Sets the projection to some kind of projection. **CAUTION** It just
   * copies the specified matrix. If you make changes to a projection, you must
   * use this function again for the changes to make effect.
   * @param proj The projection type you want.
   */
  void setProjection(Projection proj);

  /**
   * @brief Sets the ratio for the perspective projection.
   * @param ratio desired.
   */
  inline void setRatio(float ratio) { ratio_ = ratio; perspective_ = glm::perspective(glm::radians(zoom_), ratio_, 0.1f, 100.f); }

  /**
   * @brief Calculates the view matrix using Euler angles and the glm::lookAt() matrix.
   * @return The view matrix.
   */
  inline auto viewMatrix() const { return view_; }

  /**
   * @brief Increases or decreases the zoom level.
   * @param how_much you want.
   */
  void zoom(float how_much);

 private:

  /**
   * @brief Calculates the front vector from the camera's (updated) Euler angles.
   */
  void updateCameraVectors();

  /**
   * @brief Updates the view matrix.
   */
  inline void updateViewMatrix() { view_ = glm::lookAt(position_, position_ + front_, up_); }

  // view matrix
  glm::mat4 view_ {};
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
  // projection options
  float ratio_ {16.f /9.f};
  float zoom_ {45.f};
  glm::mat4 ortho_ {glm::ortho(0.f, 800.f, 600.f, 0.f, -1.f, 1.f)};
  glm::mat4 perspective_ {glm::perspective(glm::radians(zoom_), ratio_, 0.1f, 100.f)};
  // current projection matrix
  glm::mat4 projection_ {perspective_};
  Projection current_projection_ {Projection::Perspective};
};

} // namespace ktp
