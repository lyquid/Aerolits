#ifndef AEROLITS_SRC_PARTICLE_H_
#define AEROLITS_SRC_PARTICLE_H_

// #include "palette.hpp"
#include "../sdl2_wrappers/sdl2_wrappers.hpp"

namespace ktp {

namespace ParticlesAtlas {

  void loadTexture(SDL2_Renderer& ren);
  extern SDL2_Texture particles_atlas;

} // end namespace TextureAtlas

struct Vortex;

struct ParticleData {
  unsigned int start_life_{};
  
  SDL_Rect texture_rect_{};

  SDL_Color start_color_{};
  SDL_Color current_color_{};
  SDL_Color end_color_{};

  float start_size_{};
  float current_size_{};
  float end_size_{};

  SDL_FPoint start_speed_{};
  SDL_FPoint current_speed_{};
  SDL_FPoint end_speed_{};

  float rotation_{};

  float start_rotation_speed_{};
  float current_rotation_speed_{};
  float end_rotation_speed_{};

  SDL_FPoint position_{};
  
  float time_step_{0};
};

class Particle {

  friend class Emitter;

 public:

  void draw() const;
  inline Particle* getNext() const { return state_.next_; }
  void init(const ParticleData& data);
  inline bool inUse() const { return life_ > 0; }
  inline void setNext(Particle* next) { state_.next_ = next; }
  bool update(float delta_time);
  bool update(float delta_time, const Vortex& vortex);

 private:

  Particle() {}

  inline static SDL_Color interpolateColors(const SDL_Color& start_color, float time_step, const SDL_Color& end_color);

  template<typename T>
  inline static T interpolateRange(T start, T time_step, T end) { return start + (end - start) * time_step; }

  template<typename T, typename U>
  inline static T interpolateRange(T start, U time_step, T end) { return start + (end - start) * time_step; }

  unsigned int life_{};
  union {
    ParticleData live_;
    Particle* next_{nullptr};
  } state_{};
};

} // end namespace ktp

#endif // AEROLITS_SRC_PARTICLE_H_