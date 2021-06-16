#ifndef AEROLITS_SRC_INCLUDE_PLAYER_HPP_
#define AEROLITS_SRC_INCLUDE_PLAYER_HPP_

#include "game_object.hpp"
#include "graphics_component.hpp"
#include "input_component.hpp"
#include "palette.hpp"
#include "particle_system.hpp"
#include "../../kuge/kuge.hpp"
#include "../../sdl2_wrappers/sdl2_wrappers.hpp"
#include <box2d/box2d.h>
#include <memory>
#include <vector>

namespace ktp {

using FPointsVector = std::vector<SDL_FPoint>;
using Input = std::unique_ptr<InputComponent>;
using Graphics = std::unique_ptr<GraphicsComponent>;

class Emitter;

struct Laser {
  b2Body*       body_ {nullptr};
  FPointsVector render_shape_ {};
};

class Player : public GameObject {
 public:

  Player(SDL_Point& screen_size, kuge::EventBus& event_bus, b2World* world, InputComponents input);
  Player(const Player& other) = delete;
  Player(Player&& other) = delete;
  ~Player() { world_->DestroyBody(body_); }
  Player& operator=(const Player& other) = delete;
  Player& operator=(Player&& other) = delete;

  void draw(SDL2_Renderer& renderer) const;
  inline bool isAlive() const { return alive_; }
  void reset();
  void update(float delta_time);

 private:

  friend class InputComponent;
  friend class PlayerInputComponent;
  friend class PlayerGraphicsComponent;

  void checkWrap();
  void generateLaserShape();
  void generatePlayerShape();
  void resetPosition();
  void setBox2D();
  void transformRenderShape();
  void updateLasers();

  kuge::EventBus& event_bus_;
  SDL_FPoint screen_size_b2_ {};
  b2World* world_ {nullptr};
  /* Components */
  Input    input_ {};
  Graphics graphics_ {};
  /* basic attributes */
  static constexpr float kDefaultPlayerSize_ {1.2f};
  bool    alive_ {true};
  b2Body* body_ {nullptr};
  b2Vec2  delta_ {};
  float   size_ {kDefaultPlayerSize_};
  bool    thrusting_ {false};
  /* shape stuff */
  FPointsVector shape_ {};
  FPointsVector render_shape_ {};
  /* lasers */
  static constexpr float kDefaultLaserSize_ {0.15f};
  std::vector<Laser> lasers_ {};
  b2BodyDef          laser_body_def_ {};
  b2FixtureDef       laser_fixture_def_ {};
  b2PolygonShape     laser_shape_b2_ {};
  FPointsVector      laser_shape_ {};
  /* flame stuff */
  static constexpr float kDefaultFlameGrowthFactor_ {0.02f};
  static constexpr float kDefaultFlameMaxLength_ {kDefaultPlayerSize_};
  static constexpr float kDefaultFlameMinLength_ {kDefaultPlayerSize_ * 0.4f};
  float flame_growth_factor_ {kDefaultFlameGrowthFactor_};
  float flame_max_lenght_ {kDefaultFlameMaxLength_};
  FPointsVector flame_shape_ {};
  FPointsVector render_flame_shape_ {};
  /* particles stuff */
  Emitter exhaust_emitter_ {"fire", {screen_size_b2_.x, screen_size_b2_.y}};
};

} // end namespace ktp

#endif // AEROLITS_SRC_INCLUDE_PLAYER_HPP_