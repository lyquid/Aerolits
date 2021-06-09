#ifndef AEROLITS_SRC_INCLUDE_PLAYER_HPP_
#define AEROLITS_SRC_INCLUDE_PLAYER_HPP_

#include "game_object.hpp"
#include "player_input_component.hpp"
#include "player_renderer_component.hpp"
#include "palette.hpp"
#include "particle_system.hpp"
#include "../../kuge/kuge.hpp"
#include "../../sdl2_wrappers/sdl2_wrappers.hpp"
#include <box2d/box2d.h>
#include <memory>
#include <vector>

namespace ktp {

using Input = std::unique_ptr<InputComponent>;
using Renderer = std::unique_ptr<RendererComponent>;

class Emitter;

struct Laser {
  b2Body*                 body_ {nullptr};
  std::vector<SDL_FPoint> render_shape_ {};
};

class Player : public GameObject{
 public:

  Player(SDL_Point& screen_size, kuge::EventBus& event_bus, b2World* world);
  ~Player() { world_->DestroyBody(body_); }
  void draw(SDL2_Renderer& renderer) const;
  inline bool isAlive() const { return alive_; }
  void reset();
  void update(float delta_time);

 private:

  friend class PlayerInputComponent;
  friend class PlayerRendererComponent;

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
  Input input_ {};
  Renderer renderer_ {};
  /* basic attributes */
  inline static constexpr SDL_Color kDefaultPlayerColor_ {Colors::white};
  inline static constexpr float     kDefaultPlayerSize_ {1.2f};
  b2AABB  aabb_ {};
  bool    alive_ {true};
  b2Body* body_ {nullptr};
  b2Vec2  delta_ {};
  float   size_ {kDefaultPlayerSize_};
  Uint32  stabilizer_time_ {};
  bool    steering_ {true};
  bool    thrusting_ {false};
  /* shape stuff */
  std::vector<SDL_FPoint> shape_ {};
  std::vector<SDL_FPoint> render_shape_ {};
  /* lasers */
  inline static constexpr float kDefaultLaserSize_ {0.15f};
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

#endif // AEROLITS_SRC_INCLUDE_PLAYER_HPP_