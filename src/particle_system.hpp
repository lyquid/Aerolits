#ifndef AEROLITS_SRC_PARTICLE_SYSTEM_HPP_
#define AEROLITS_SRC_PARTICLE_SYSTEM_HPP_

#include "emitter_parser.hpp"
#include "particle.hpp"
#include "../sdl2_wrappers/sdl2_wrappers.hpp"
#include <utility> // std::move

namespace ktp {

/* http://gameprogrammingpatterns.com/object-pool.html */
/* https://nintervik.github.io/2D-Particle-System/ */
/* https://gamedevelopment.tutsplus.com/tutorials/adding-turbulence-to-a-particle-system--gamedev-13332 */

inline int generateRand(int min, int max);
inline float generateRand(float min, float max);
inline double generateRand(double min, double max);

struct Vortex {
  SDL_FPoint position_{};
  float scale_{};
  float speed_{};
};

class Emitter {
 public:
  
  Emitter(EmitterTypes type, const SDL_FPoint& pos) noexcept;
  Emitter(const Emitter& other) noexcept { *this = other; }
  Emitter(Emitter&& other) noexcept { *this = std::move(other); }
  ~Emitter() noexcept { delete[] particles_pool_; }

  Emitter& operator=(const Emitter& other) noexcept;
  Emitter& operator=(Emitter&& other) noexcept;

  inline bool canBeDeleted() const { return lifeTimeOver() && !alive_particles_count_; }
  void draw() const;
  void generateParticles();
  inline auto getAliveParticlesCount() const { return alive_particles_count_; }
  inline SDL_FPoint getPosition() const { return position_; }
  inline void setPosition(const SDL_FPoint& pos) { position_ = pos; }
  inline bool particlesAlive() const { return alive_particles_count_ != 0u; }
  inline bool lifeTimeOver() const { return SDL2_Timer::getSDL2Ticks() - start_time_ >= data_->life_time_; }
  void update(float delta_time);

 private:

  void inflatePool();

  unsigned int alive_particles_count_{0};
  const EmitterType* data_{nullptr};
  Particle* first_available_{nullptr};
  Particle* particles_pool_{nullptr};
  unsigned int particles_pool_size_{0};
  SDL_FPoint position_{};
  Uint32 start_time_{SDL2_Timer::getSDL2Ticks()};
  Uint32 interval_time_{};
};

} // end namespace ktp

#endif // AEROLITS_SRC_PARTICLE_SYSTEM_HPP_