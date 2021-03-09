#ifndef AEROLITS_SRC_PARTICLE_SYSTEM_H_
#define AEROLITS_SRC_PARTICLE_SYSTEM_H_

#include "palette.h"
#include "../sdl2_wrappers/sdl2_wrappers.h"
#include <string>

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
 
  ParticlePool(int pool_size);
  ParticlePool(const ParticlePool& other) = delete;
  ~ParticlePool() { delete[] particles_; }

  void operator=(const ParticlePool& other) = delete;

  void draw(const SDL2_Renderer& renderer) const;
  void generate(const SDL_FPoint& pos, const SDL_FPoint& delta, int life_time);
  void update(float delta_time);

 private:

  const int kPoolSize_;
  Particle* first_available_{nullptr};
  Particle* particles_{nullptr};
};

/* 
<emitter type ="fire">
  <angleRange min ="80" max ="110"/>

  <rotSpeed value ="0" randMin = "-1" randMax ="1"/>
  <startSpeed value ="200" randMin = "0" randMax ="1"/>
  <endSpeed value ="200" randMin = "0" randMax ="1"/>
  <startSize value ="200" randMin ="0" randMax ="1"/>
  <endSize value =" 80" randMin ="1" randMax ="1"/>

  <emitNumber value ="1"/>

  <emitVariance value ="0" randMin = "0" randMax ="1"/>
  <maxParticleLife value ="100" randMin = "0" randMax ="1"/>

  <textureRect x="0" y="0" w="128" h="128"/>
  
  <startColor r ="255" g="100" b="0" a="255"/>
  <endColor r ="210" g="210" b="210" a ="0"/>

  <blendMode mode ="add"/>
  <lifetime value ="-1"/>
</emitter> */


enum class EmitterTypes {
  Fire,
  Smoke,
  count
};

struct AngleRange { float max_{0}, min_{0}; };

struct ValueWithRnd {
  float value_{0};
  int rand_min_{0}, rand_max_{1};
};

class Emitter {
 public:
  
  //Emitter(EmitterTypes type);
  inline void draw(const SDL2_Renderer& renderer) const { particle_pool_.draw(renderer); }
  inline void generate(const SDL_FPoint& pos, const SDL_FPoint& delta, int life_time) { particle_pool_.generate(pos, delta, life_time); }
  inline void update(float delta_time) { particle_pool_.update(delta_time); }

 private:

  ParticlePool particle_pool_{10000};
  // from the xml 
  EmitterTypes type_{};
  AngleRange angle_range_{};

  ValueWithRnd rotation_speed_{};
  ValueWithRnd start_speed_{};
  ValueWithRnd end_speed_{};
  ValueWithRnd start_size_{};
  ValueWithRnd end_size_{};

  int emit_number_{};

  ValueWithRnd emit_variance_{};
  ValueWithRnd max_particle_life_{};

  SDL_Rect texture_rect_{};

  SDL_Color start_color_{};
  SDL_Color end_color_{};

  SDL_BlendMode blend_mode_{};
  /* seconds (?) an emitter lives */
  int life_time_{};
};

} // end namespace ktp

#endif // AEROLITS_SRC_PARTICLE_SYSTEM_H_