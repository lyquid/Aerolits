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

  Aerolite(const SDL_FPoint& where) noexcept;
  
  static void setB2World(b2World* world) { b2_world_ = world; }
  static void setScreenSize(const SDL_Point& screen_size);

  static Aerolite spawnAerolite();

  inline bool canBeDeleted() const { return to_be_deleted_; }
  void draw(const SDL2_Renderer& renderer) const;
  void update(float delta_time);

 private:

  void generateAeroliteShape(float size);
  void transformRenderShape();
  
  inline static SDL_Point screen_size_ {};
  inline static SDL_Point screen_size_b2_ {};

  inline static b2World* b2_world_ {nullptr};

  inline static constexpr float kKgPerMeter2_ {20.f};
  inline static constexpr float kMaxRotationSpeed_ {1.f};
  inline static constexpr float kMaxSize_ {5.f};
  inline static constexpr float kMaxSpeed_ {2.f};

  b2Body* body_ {nullptr};
  std::vector<SDL_FPoint> shape_ {};
  std::vector<SDL_FPoint> render_shape_ {};
  bool to_be_deleted_ {false};
};

} // namespace ktp

#endif // AEROLITS_SRC_AEROLITES_HPP_