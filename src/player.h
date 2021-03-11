#ifndef AEROLITS_SRC_PLAYER_H_
#define AEROLITS_SRC_PLAYER_H_

#include "palette.h"
#include "particle_system.h"
#include "../kuge/kuge.h"
#include "../sdl2_wrappers/sdl2_wrappers.h"
#include <vector>

namespace ktp {

struct Laser {
  float                   angle_{0};
  SDL_FPoint              delta_{0, 0};
  std::vector<SDL_FPoint> shape_{};
  float                   size_{20};
};

class Player {
 public:

  Player(SDL_Point& screen_size, kuge::EventBus& event_bus);
  void draw(SDL2_Renderer& renderer) const;
  inline bool isAlive() const { return alive_; }
  void reset();
  void shoot();
  inline void steerLeft(float delta_time)  { angle_ -= 5.f * delta_time; }
  inline void steerRight(float delta_time) { angle_ += 5.f * delta_time; }
  void stopThrusting();
  void thrust(float delta_time);
  void update(float delta_time);

 private:

  void generatePlayerShape();
  void move(float delta_time);
  void resetPosition();
  void rotate();
  void updateLasers(float delta_time);
  
  kuge::EventBus& event_bus_;
  SDL_Point& screen_size_;

  /* basic attributes */
  const float kMaxDelta_{5000.f};
  const SDL_Color kDefaultPlayerColor_{Colors::white};
  bool       alive_{true};
  float      angle_{0.f};
  SDL_FPoint center_{};
  SDL_FPoint delta_{0.f, 0.f};
  float      size_{50.f};
  /* shape stuff */
  std::vector<SDL_FPoint> shape_{};
  std::vector<SDL_FPoint> render_shape_{};
  /* shooting stuff */
  const float kDefaultLaserSpeed_{1000.f};
  const float kDefaultShootingInterval_{200.f};
  std::vector<Laser> lasers_{};
  double shooting_timer_{0.f};
  /* thrust stuff */
  const float kDefaultThrustSpeed_{200.f};
  float thrust_speed_{kDefaultThrustSpeed_};
  bool thrusting_{false};
  /* flame stuff */
  const float kDefaultFlameGrowthFactor_{0.2f};
  const float kDefaultFlameMaxLength_{size_};
  const float kDefaultFlameMinLength_{size_ * 0.4f};
  float flame_growth_factor_{kDefaultFlameGrowthFactor_};
  float flame_max_lenght_{kDefaultFlameMaxLength_};
  std::vector<SDL_FPoint> flame_shape_{};
  std::vector<SDL_FPoint> render_flame_shape_{};
  /* particles stuff */
  Emitter emitter_{EmitterTypes::Fire};
};

} // end namespace ktp

#endif // AEROLITS_SRC_PLAYER_H_