#ifndef AEROLITS_SRC_PARTICLE_SYSTEM_H_
#define AEROLITS_SRC_PARTICLE_SYSTEM_H_

#include "emitter_parser.hpp"
#include "particle.hpp"
#include "../sdl2_wrappers/sdl2_wrappers.hpp"
#include <utility>

namespace ktp {

/* http://gameprogrammingpatterns.com/object-pool.html */
/* https://nintervik.github.io/2D-Particle-System/ */
/* https://gamedevelopment.tutsplus.com/tutorials/adding-turbulence-to-a-particle-system--gamedev-13332 */

inline int generateRand(int min, int max);
inline float generateRand(float min, float max);
inline double generateRand(double min, double max);

class Emitter {
 public:
  
  Emitter(EmitterTypes type, const SDL_FPoint& pos) noexcept;
  Emitter(const Emitter& other) noexcept { *this = other; }
  Emitter(Emitter&& other) noexcept { *this = std::move(other); }
  ~Emitter() noexcept { delete[] particles_pool_; }

  Emitter& operator=(const Emitter& other) noexcept;
  Emitter& operator=(Emitter&& other) noexcept;

  void draw() const;
  void generateParticles();
  inline SDL_FPoint getPosition() const { return position_; }
  inline void setPosition(const SDL_FPoint& pos) { position_ = pos; }
  void update(float delta_time);

 private:

  void inflatePool();

  EmitterType data_{};
  Particle* first_available_{nullptr};
  Particle* particles_pool_{nullptr};
  unsigned int particles_pool_size_{1000};
  SDL_FPoint position_{};
};

} // end namespace ktp

#endif // AEROLITS_SRC_PARTICLE_SYSTEM_H_