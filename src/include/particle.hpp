#ifndef AEROLITS_SRC_INCLUDE_PARTICLE_HPP_
#define AEROLITS_SRC_INCLUDE_PARTICLE_HPP_

#include <glm/glm.hpp>
#include <SDL.h>
#include <utility> // std::move
#include <vector>

namespace ktp {

using GLMColors     = std::vector<glm::vec4>;
using FPointsVector = std::vector<SDL_FPoint>;
using SizeVector    = std::vector<float>;

struct Vortex;

struct ParticleData {
  unsigned int  start_life_ {};
  SDL_Rect      texture_rect_ {};
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
  SDL_FPoint    position_ {};
  float         time_step_ {};
};

class Particle {
 public:

  Particle() = default;
  Particle(const Particle& other) { *this = other; }
  Particle(Particle&& other) { *this = std::move(other); }

  Particle& operator=(const Particle& other);
  Particle& operator=(Particle&& other);

  Particle* getNext() const { return state_.next_; }
  void init(const ParticleData& data);
  bool inUse() const { return life_ > 0; }
  void setNext(Particle* next) { state_.next_ = next; }
  bool update(glm::vec3& pos, glm::vec4& color);
  bool update(const Vortex& vortex, glm::vec3& pos, glm::vec4& color);

  inline static glm::vec4 interpolate2Colors(const glm::vec4& start_color, const glm::vec4& end_color, float time_step);
  inline static glm::vec4 interpolate3Colors(const glm::vec4& start_color, const glm::vec4& mid_color, const glm::vec4& end_color, float time_step);

  template<typename T>
  inline static T interpolateRange(T start, T end, T time_step) { return start + (end - start) * time_step; }

  template<typename T, typename U>
  inline static T interpolateRange(T start, T end, U time_step) { return start + (end - start) * time_step; }

  template<typename T>
  inline static T interpolateRange3(T start, T mid, T end, T time_step) {
    if (time_step < 0.5) {
      return (mid * time_step * 2.0) + start * (0.5 - time_step) * 2.0;
    } else {
      return end * (time_step - 0.5) * 2.0 + mid * (1.0 - time_step) * 2.0;
    }
  }

  template<typename T, typename U>
  inline static T interpolateRange3(T start, T mid, T end, U time_step) {
    if (time_step < 0.5) {
      return (mid * time_step * 2.0) + start * (0.5 - time_step) * 2.0;
    } else {
      return end * (time_step - 0.5) * 2.0 + mid * (1.0 - time_step) * 2.0;
    }
  }

 private:

  unsigned int life_ {};

  union State {
    ~State() {}
    State& operator=(const State& other);
    State& operator=(State&& other);

    ParticleData live_;
    Particle* next_ {nullptr};
  } state_ {};
};

} // end namespace ktp

#endif // AEROLITS_SRC_INCLUDE_PARTICLE_HPP_
