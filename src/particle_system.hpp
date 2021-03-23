#ifndef AEROLITS_SRC_PARTICLE_SYSTEM_H_
#define AEROLITS_SRC_PARTICLE_SYSTEM_H_

#include "emitter_parser.hpp"
#include "particle.hpp"
#include "../sdl2_wrappers/sdl2_wrappers.hpp"
#include <random>
#include <string>
#include <vector>

namespace ktp {

/* http://gameprogrammingpatterns.com/object-pool.html */
/* https://nintervik.github.io/2D-Particle-System/ */
/* https://gamedevelopment.tutsplus.com/tutorials/adding-turbulence-to-a-particle-system--gamedev-13332 */

class ParticlePool {
 public:
 
  ParticlePool(int pool_size);
  ParticlePool(const ParticlePool& other) = delete;
  ~ParticlePool() { delete[] particles_; }

  void operator=(const ParticlePool& other) = delete;

  void draw() const;
  void generate(const ParticleData& data);
  void update(float delta_time);

 private:

  const int kPoolSize_;
  Particle* first_available_{nullptr};
  Particle* particles_{nullptr};
};

class Emitter {
 public:
  
  Emitter(EmitterTypes type, const SDL_FPoint& pos);
  inline void draw() const { particle_pool_.draw(); }
  void generate();
  inline SDL_FPoint getPosition() const { return position_; }
  inline void setPosition(const SDL_FPoint& pos) { position_ = pos; }
  inline void update(float delta_time) { particle_pool_.update(delta_time); }

  inline static int generateRand(int min, int max);
  inline static float generateRand(float min, float max);
  inline static double generateRand(double min, double max);

 private:

  ParticlePool particle_pool_{10000};
  SDL_FPoint position_{};

  // from the xml 
  EmitterTypes type_{};
  RRVUint max_particle_life_{};
  SDL_Rect texture_rect_{};
  SDL_BlendMode blend_mode_{};
  RRVFloat start_size_{};
  RRVFloat end_size_{};
  SDL_Color start_color_{};
  SDL_Color end_color_{};
  RRVFloat rotation_{};
  RRVFloat start_rotation_speed_{};
  RRVFloat end_rotation_speed_{};



  AngleRange angle_range_{};
  RRVFloat start_speed_{};
  RRVFloat end_speed_{};
  int emit_number_{};
  RRVInt emit_variance_{};
  // emitter max life
  int life_time_{};
};

} // end namespace ktp

#endif // AEROLITS_SRC_PARTICLE_SYSTEM_H_