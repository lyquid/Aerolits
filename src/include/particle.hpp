#ifndef AEROLITS_SRC_INCLUDE_PARTICLE_HPP_
#define AEROLITS_SRC_INCLUDE_PARTICLE_HPP_

#include <glm/glm.hpp>
#include <SDL.h>
#include <utility> // std::move
#include <vector>

namespace ktp {

using GLMPositions  = std::vector<glm::vec3>;
using GLMColors     = std::vector<glm::vec4>;
using FPointsVector = std::vector<SDL_FPoint>;
using SizeVector    = std::vector<float>;

struct ParticleData {
  float         start_life_ {};
  GLMColors     colors_ {};
  glm::vec4     current_color_ {};
  SizeVector    sizes_ {};
  float         current_size_ {};
  FPointsVector speeds_ {};
  SDL_FPoint    current_speed_ {};
  float         rotation_ {};
  float         start_rotation_speed_ {};
  float         current_rotation_speed_ {};
  float         end_rotation_speed_ {};
  glm::vec3     position_ {};
  float         time_step_ {};
};

class Particle {
 public:

  Particle* getNext() const { return next_; }
  void init(const ParticleData& data);
  bool inUse() const { return current_life_ > 0; }
  void setNext(Particle* next) { next_ = next; }
  bool update(float delta_time, GLfloat* subdata);

  static inline glm::vec4 interpolate2Colors(const glm::vec4& start_color, const glm::vec4& end_color, float time_step);
  static inline glm::vec4 interpolate3Colors(const glm::vec4& start_color, const glm::vec4& mid_color, const glm::vec4& end_color, float time_step);

  template<typename T>
  static T interpolateRange(T start, T end, T time_step) { return start + (end - start) * time_step; }

  template<typename T, typename U>
  static T interpolateRange(T start, T end, U time_step) { return start + (end - start) * time_step; }

  template<typename T>
  static T interpolateRange3(T start, T mid, T end, T time_step) {
    if (time_step < 0.5f) {
      return (mid * time_step * 2.f) + start * (0.5f - time_step) * 2.f;
    } else {
      return end * (time_step - 0.5f) * 2.f + mid * (1.f - time_step) * 2.f;
    }
  }

  template<typename T, typename U>
  static T interpolateRange3(T start, T mid, T end, U time_step) {
    if (time_step < 0.5) {
      return (mid * time_step * 2.0) + start * (0.5 - time_step) * 2.0;
    } else {
      return end * (time_step - 0.5) * 2.0 + mid * (1.0 - time_step) * 2.0;
    }
  }

 private:

  // life
  float start_life_ {};
  float current_life_ {};
  // colors
  GLMColors colors_ {};
  glm::vec4 current_color_ {};
  // sizes
  SizeVector sizes_ {};
  float      current_size_ {};
  // speeds
  FPointsVector speeds_ {};
  SDL_FPoint    current_speed_ {};
  // rotations
  float rotation_ {};
  float start_rotation_speed_ {};
  float current_rotation_speed_ {};
  float end_rotation_speed_ {};
  // position
  glm::vec3 position_ {};
  // timestep
  float time_step_ {};
  // next free particle
  Particle* next_ {nullptr};
};

} // end namespace ktp

#endif // AEROLITS_SRC_INCLUDE_PARTICLE_HPP_
