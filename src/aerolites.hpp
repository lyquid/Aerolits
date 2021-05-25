#ifndef AEROLITS_SRC_AEROLITES_HPP_
#define AEROLITS_SRC_AEROLITES_HPP_

#include "../sdl2_wrappers/sdl2_wrappers.hpp"
#include "../include/random.hpp"

namespace ktp {

enum class SpawnSides {
  Top, Right, Down, Left, count
};

class Aerolite {
 public:

  static void setScreenSize(const SDL_Point& screen_size) { kScreenSize = screen_size; }
  static Aerolite spawnAerolite(const SDL_Point& screen_size);

  inline bool canBeDeleted() const { return to_be_deleted_; }
  void draw(const SDL2_Renderer& renderer) const;
  inline SDL_FPoint getPosition() const { return position_; }
  void update(float delta_time);

 private:

  inline static constexpr unsigned int kMaxSize_ {200u};
  inline static constexpr float kMaxSpeed_ {150.f};
  inline static SDL_Point kScreenSize {};

  SDL_FPoint delta_ {};
  SDL_FPoint position_ {};
  SDL_Rect   shape_ {};
  float      speed_ {kMaxSpeed_ * generateRand(0.5f, 1.f)};
  bool       to_be_deleted_ {};
};


} // namespace ktp

#endif // AEROLITS_SRC_AEROLITES_HPP_