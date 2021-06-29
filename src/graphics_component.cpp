#include "include/game_entity.hpp"
#include "include/graphics_component.hpp"
#include "include/physics_component.hpp"
#include "include/particle.hpp"
#include "../sdl2_wrappers/sdl2_renderer.hpp"
#include "../sdl2_wrappers/sdl2_texture.hpp"

/* AEROLITE */

void ktp::AeroliteGraphicsComponent::update(const GameEntity& aerolite, const SDL2_Renderer& renderer) {
  renderer.setDrawColor(kDefaultColor_);
  renderer.drawLines(render_shape_);
}

/* EMITTER */

void ktp::EmitterGraphicsComponent::update(const GameEntity& emitter, const SDL2_Renderer& renderer) {
  ParticlesAtlas::particles_atlas.setBlendMode(blend_mode_);
  for (auto i = 0u; i < particles_pool_size_; ++i) {
    if (particles_pool_[i].inUse()) {
      particles_pool_[i].draw();
    }
  }
}

/* PLAYER */

void ktp::PlayerGraphicsComponent::update(const GameEntity& player, const SDL2_Renderer& renderer) {
  // player's shape
  renderer.setDrawColor(kDefaultColor_);
  renderer.drawLines(render_shape_);
  // player's thrust fx
  if (thrusting_) {
    renderer.setDrawColor(Colors::red);
    renderer.drawLines(render_flame_shape_);
  }
  // particles
  // player.exhaust_emitter_.draw();
}

/* PROJECTILE */

void ktp::ProjectileGraphicsComponent::update(const GameEntity& projectile, const SDL2_Renderer& renderer) {
  renderer.setDrawColor(kDefaultColor_);
  renderer.drawLines(render_shape_);
}
