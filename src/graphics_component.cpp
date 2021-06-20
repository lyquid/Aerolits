#include "./include/graphics_component.hpp"
#include "../sdl2_wrappers/sdl2_renderer.hpp"

/* PLAYER RENDER */

void ktp::PlayerGraphicsComponent::update(const GameEntity& player, const SDL2_Renderer& renderer) {
  // player's shape
  renderer.setDrawColor(kDefaultPlayerColor_);
  renderer.drawLines(render_shape_);
  // player's thrust fx
  if (thrusting_) {
    renderer.setDrawColor(Colors::red);
    renderer.drawLines(render_flame_shape_);
  }
  // particles
  // player.exhaust_emitter_.draw();
  // lasers
  /* renderer.setDrawColor(Colors::copper_green);
  for (const auto& laser: player.lasers_) {
    renderer.drawLines(laser.render_shape_);
  } */
}

/* AEROLITES */

void ktp::AeroliteGraphicsComponent::update(const GameEntity& aerolite, const SDL2_Renderer& renderer) {
  renderer.setDrawColor(kDefaultColor_);
  renderer.drawLines(render_shape_);
}