#ifndef AEROLITS_SRC_PLAYER_H_
#define AEROLITS_SRC_PLAYER_H_

#include "../kuge/kuge.h"
#include "../sdl2_wrappers/sdl2_wrappers.h"

#include <vector>

namespace ktp {

struct Laser {
  float                   angle_ = 0;
  SDL_FPoint              delta_{0, 0};
  std::vector<SDL_FPoint> shape_{};
  float                   size_ = 20;
};

class Player {
 public:
  Player(SDL_Point& screen_size, kuge::EventBus& event_bus);
  void draw(SDL2_Renderer& renderer);
  bool isAlive() const { return alive_; }
  void reset();
  void shoot();
  void steerLeft(float delta_time)  { angle_ -= 5.f * delta_time; }
  void steerRight(float delta_time) { angle_ += 5.f * delta_time; };
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
  const float kMaxDelta_ = 5000.f;
  const SDL_Color kDefaultPlayerColor_{245, 245, 245, 255};
  float      size_ = 50;
  bool       alive_ = true;
  float      angle_ = 0.f;
  SDL_FPoint center_{};
  SDL_FPoint delta_{0, 0};
  /* shape stuff */
  std::vector<SDL_FPoint> shape_{};
  std::vector<SDL_FPoint> render_shape_{};
  /* shooting stuff */
  std::vector<Laser> lasers_{};
  double shooting_timer_ = 200.f;
};

} // end namespace ktp

#endif // AEROLITS_SRC_PLAYER_H_