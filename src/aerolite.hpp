#ifndef AEROLITS_SRC_AEROLITES_HPP_
#define AEROLITS_SRC_AEROLITES_HPP_

#include "../sdl2_wrappers/sdl2_wrappers.hpp"
#include "../include/random.hpp"

#include <box2d/box2d.h>
#include <vector>

namespace ktp {

enum class SpawnSides {
  Top, Right, Down, Left, count
};

class Aerolite {
 public:

  Aerolite() { generateAeroliteShape(kMaxSize_ * generateRand(0.3f, 1.f)); }
  // ~Aerolite() { b2_world_->DestroyBody(body_); }

  static void setB2World(b2World* world) { b2_world_ = world; }
  static void setScreenSize(const SDL_Point& screen_size) { screen_size_ = screen_size; }
  static Aerolite spawnAerolite(const SDL_Point& screen_size);
  static Aerolite spawnAerolite2();

  inline bool canBeDeleted() const { return to_be_deleted_; }
  void draw(const SDL2_Renderer& renderer) const;
  inline b2Body* getB2Body() const { return body_; }
  // inline SDL_FPoint getPosition() const { return position_; }
  inline b2Vec2 getPosition() const { return body_->GetPosition(); }
  void update(float delta_time);

 private:

  void generateAeroliteShape(int size);
  void move(float delta_time);
  void rotate();

  inline static SDL_Point screen_size_ {};
  inline static b2World* b2_world_ {nullptr};

  /* shape stuff */
  inline static constexpr float kMaxSize_ {200.f};
  inline static constexpr float kMaxSpeed_ {20.f};
  std::vector<SDL_FPoint> shape_ {};
  std::vector<SDL_FPoint> render_shape_ {};
  SDL_FPoint delta_ {kMaxSpeed_ * generateRand(-1.f, 1.f), kMaxSpeed_ * generateRand(-1.f, 1.f)};
  /* stuff for rotation */
  inline static constexpr float kMaxRotationSpeed_ {0.01f};
  SDL_FPoint center_ {screen_size_.x / 2, screen_size_.y / 2};
  float angle_ {generateRand(-180.f, 180.f)};
  float rotation_speed_ {kMaxRotationSpeed_ * generateRand(-1.f, 1.f)};
  
  bool to_be_deleted_ {false};
  // box2d
  b2Body* body_ {nullptr};
};


} // namespace ktp

#endif // AEROLITS_SRC_AEROLITES_HPP_