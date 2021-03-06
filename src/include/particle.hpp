#ifndef AEROLITS_SRC_INCLUDE_PARTICLE_HPP_
#define AEROLITS_SRC_INCLUDE_PARTICLE_HPP_

#include <SDL.h>
#include <utility> // std::move
#include <vector>

namespace ktp {

class SDL2_Renderer;
class SDL2_Texture;

namespace ParticlesAtlas {

  void loadTexture(SDL2_Renderer& ren);
  extern SDL2_Texture particles_atlas;

} // end namespace TextureAtlas

using ColorsVector = std::vector<SDL_Color>;
using FPointsVector = std::vector<SDL_FPoint>;
using SizeVector = std::vector<float>;

struct Vortex;

struct ParticleData {
  unsigned int  start_life_{};
  SDL_Rect      texture_rect_{};
  ColorsVector  colors_{};
  SDL_Color     current_color_{};
  SizeVector    sizes_{};
  float         current_size_{};
  FPointsVector speeds_{};
  SDL_FPoint    current_speed_{};
  float         rotation_{};
  float         start_rotation_speed_{};
  float         current_rotation_speed_{};
  float         end_rotation_speed_{};
  SDL_FPoint    position_{};
  float         time_step_{};
};

class Particle {

  friend class Emitter;
  friend class EmitterGraphicsComponent;
  friend class EmitterPhysicsComponent;

 public:

  void draw() const;
  inline Particle* getNext() const { return state_.next_; }
  void init(const ParticleData& data);
  inline bool inUse() const { return life_ > 0; }
  inline void setNext(Particle* next) { state_.next_ = next; }
  bool update();
  bool update(const Vortex& vortex);

  Particle& operator=(const Particle& other) noexcept;
  Particle& operator=(Particle&& other) noexcept;

 private:

  Particle() {}
  Particle(const Particle& other) noexcept { *this = other; }
  Particle(Particle&& other) noexcept { *this = std::move(other); }
  ~Particle() {}

  inline static SDL_Color interpolate2Colors(const SDL_Color& start_color, const SDL_Color& end_color, float time_step);

  inline static SDL_Color interpolate3Colors(const SDL_Color& start_color, const SDL_Color& mid_color, const SDL_Color& end_color, float time_step);

  template<typename T>
  inline static T interpolateRange(T start,T end, T time_step) { return start + (end - start) * time_step; }

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

  unsigned int life_{};
  union State {
    ~State() {}
    State& operator=(const State& other) noexcept;
    State& operator=(State&& other) noexcept;

    ParticleData live_;
    Particle* next_{nullptr};
  } state_{};
};

} // end namespace ktp

#endif // AEROLITS_SRC_INCLUDE_PARTICLE_HPP_