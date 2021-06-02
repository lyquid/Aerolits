#ifndef AEROLITS_SRC_PLAYER_H_
#define AEROLITS_SRC_PLAYER_H_

#include "palette.hpp"
#include "particle_system.hpp"
#include "../kuge/kuge.hpp"
#include "../sdl2_wrappers/sdl2_wrappers.hpp"
#include <box2d/box2d.h>
#include <vector>

namespace ktp {

struct Laser {
  b2Body*                 body_ {nullptr};
  std::vector<SDL_FPoint> render_shape_ {};
};

class Player {
 public:

  Player(SDL_Point& screen_size, kuge::EventBus& event_bus);
  void draw(SDL2_Renderer& renderer) const;
  inline bool isAlive() const { return alive_; }
  void reset();
  void setBox2d(b2World* world);
  void shoot();
  void steerLeft();
  void steerRight();
  void stopThrusting();
  void thrust(float delta_time);
  void update(float delta_time);

 private:

  void checkWrap();
  void generateLaserShape();
  void generatePlayerShape();
  void resetPosition();
  void transformRenderShape();
  void updateLasers();
  
  kuge::EventBus& event_bus_;
  SDL_FPoint screen_size_b2_ {};
  b2World* world_ {nullptr};

  /* basic attributes */
  inline static constexpr float     kDefaultAngularImpulse_ {5.f};
  inline static constexpr float     kDefaultLinearImpulse_ {0.05f};
  inline static constexpr SDL_Color kDefaultPlayerColor_ {Colors::white};
  inline static constexpr float     kDefaultPlayerSize_ {1.2f};
  inline static constexpr float     kMaxDelta_ {0.1f};
  bool    alive_ {true};
  float   angular_impulse_ {kDefaultAngularImpulse_};
  b2Body* body_ {nullptr};
  b2Vec2  delta_ {};
  float   linear_impulse_ {kDefaultLinearImpulse_};
  float   size_ {kDefaultPlayerSize_};
  Uint32  stabilizer_time_ {};
  bool    steering_ {true};
  bool    thrusting_ {false};
  /* shape stuff */
  std::vector<SDL_FPoint> shape_ {};
  std::vector<SDL_FPoint> render_shape_ {};
  /* shooting stuff */
  inline static constexpr float kDefaultShootingInterval_ {200.f};
  double shooting_timer_ {};
  /* lasers */
  inline static constexpr float kDefaultLaserSize_ {0.15f};
  inline static constexpr float kDefaultLaserSpeed_ {30.f};
  std::vector<Laser> lasers_ {};
  b2BodyDef          laser_body_def_ {};
  b2FixtureDef       laser_fixture_def_ {};
  b2PolygonShape     laser_shape_b2_ {};
  FPointsVector      laser_shape_ {};
  /* flame stuff */
  inline static constexpr float kDefaultFlameGrowthFactor_ {0.02f};
  inline static constexpr float kDefaultFlameMaxLength_ {kDefaultPlayerSize_};
  inline static constexpr float kDefaultFlameMinLength_ {kDefaultPlayerSize_ * 0.4f};
  float flame_growth_factor_ {kDefaultFlameGrowthFactor_};
  float flame_max_lenght_ {kDefaultFlameMaxLength_};
  std::vector<SDL_FPoint> flame_shape_ {};
  std::vector<SDL_FPoint> render_flame_shape_ {};
  /* particles stuff */
  Emitter exhaust_emitter_ {"fire", {screen_size_b2_.x, screen_size_b2_.y}};
};

} // end namespace ktp

#endif // AEROLITS_SRC_PLAYER_H_