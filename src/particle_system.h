#ifndef AEROLITS_SRC_PARTICLE_SYSTEM_H_
#define AEROLITS_SRC_PARTICLE_SYSTEM_H_

#include "palette.h"
#include "../sdl2_wrappers/sdl2_wrappers.h"

namespace ktp {

/* http://gameprogrammingpatterns.com/object-pool.html */
/* https://nintervik.github.io/2D-Particle-System/ */
/* https://gamedevelopment.tutsplus.com/tutorials/adding-turbulence-to-a-particle-system--gamedev-13332 */

class Particle {

  friend class ParticlePool;

 public:

  void draw(const SDL2_Renderer& renderer) const;
  inline Particle* getNext() const { return state_.next_; }
  void init(const SDL_FPoint& pos, const SDL_FPoint& delta, int life_time);
  inline bool inUse() const { return frames_left_ > 0; }
  void setNext(Particle* next) { state_.next_ = next; }
  bool update(float delta_time);

 private:

  Particle() {}

  int frames_left_{0};
  union {
    struct {
      SDL_FPoint delta_{};
      SDL_FPoint position_{};
    } live_;
    Particle* next_{nullptr};
  } state_{};
};

class ParticlePool {
 public:
 
  ParticlePool();
  void draw(const SDL2_Renderer& renderer) const;
  void generate(const SDL_FPoint& pos, const SDL_FPoint& delta, int life_time);
  void update(float delta_time);

 private:

  static const int kPoolSize_{10000};
  Particle* first_available_{nullptr};
  Particle particles_[kPoolSize_];
};

} // end namespace ktp

#endif // AEROLITS_SRC_PARTICLE_SYSTEM_H_