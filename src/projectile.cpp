#include "include/box2d_scale.hpp"
#include "include/game.hpp"
#include "include/game_entity.hpp"
#include "include/projectile.hpp"
#include "../sdl2_wrappers/sdl2_renderer.hpp"
#include "../sdl2_wrappers/sdl2_timer.hpp"
#include <box2d/box2d.h>

/* GRAPHICS */

void ktp::ProjectileGraphicsComponent::update(const GameEntity& projectile, const SDL2_Renderer& renderer) {
  renderer.setDrawColor(kDefaultColor_);
  renderer.drawLines(render_shape_);
}

/* PHYSICS */

ktp::ProjectilePhysicsComponent::ProjectilePhysicsComponent(GameEntity* owner, ProjectileGraphicsComponent* graphics) noexcept:
 graphics_(graphics) {
  owner_ = owner;
  size_ = kDefaultProjectileSize_;
  generateProjectileShape(shape_, size_);
  graphics_->render_shape_.resize(shape_.size());

  b2BodyDef body_def {};
  body_def.type = b2_dynamicBody;
  body_def.bullet = true;
  body_def.userData.pointer = reinterpret_cast<uintptr_t>(owner_);

  body_ = world_->CreateBody(&body_def);

  b2Vec2 laser_vertices[4];
  laser_vertices[0].Set(-size_ * 0.15f, -size_); // top left
  laser_vertices[1].Set(-size_ * 0.15f,  size_); // down left
  laser_vertices[2].Set( size_ * 0.15f,  size_); // down right
  laser_vertices[3].Set( size_ * 0.15f, -size_); // up right

  b2PolygonShape projectile_shape {};
  projectile_shape.Set(laser_vertices, 4);

  b2FixtureDef projectile_fixture_def {};
  projectile_fixture_def.shape = &projectile_shape;
  projectile_fixture_def.density = 10.f;
  projectile_fixture_def.friction = 0.1f;
  projectile_fixture_def.restitution = 0.35f;

  body_->CreateFixture(&projectile_fixture_def);
}

ktp::ProjectilePhysicsComponent& ktp::ProjectilePhysicsComponent::operator=(ProjectilePhysicsComponent&& other) noexcept {
  if (this != &other) {
    // inherited members
    body_     = other.body_;
    collided_ = other.collided_;
    delta_    = std::move(other.delta_);
    owner_    = std::exchange(other.owner_, nullptr);
    shape_    = std::move(other.shape_);
    size_     = other.size_;
    // own members
    blast_power_         = other.blast_power_;
    detonated_           = other.detonated_;
    explosion_particles_ = std::move(other.explosion_particles_);
    explosion_time_      = other.explosion_time_;
    graphics_            = std::exchange(other.graphics_, nullptr);
  }
  return *this;
}

void ktp::ProjectilePhysicsComponent::detonate() {
  detonated_ = true;
  explosion_time_ = Game::gameplay_timer_.getTicks();
  for (std::size_t i = 0; i < kExplosionRays_; ++i) {
    const float angle = (i / (float)kExplosionRays_) * 360 * (kPI / 180);
    const b2Vec2 ray_dir {sinf(angle), cosf(angle)};

    b2BodyDef bd;
    bd.bullet = true;
    bd.fixedRotation = true;
    bd.linearDamping = 5.f;
    bd.linearVelocity = blast_power_ * ray_dir;
    bd.position = body_->GetPosition();
    bd.type = b2_dynamicBody;
    bd.userData.pointer = 0;

    auto body {world_->CreateBody(&bd)};

    b2CircleShape circle_shape;
    circle_shape.m_radius = 0.2f; // before 0.05f

    b2FixtureDef fd;
    fd.density = 60.f;
    fd.filter.groupIndex = -1;
    fd.friction = 0.f;
    fd.restitution = 0.99f;
    fd.shape = &circle_shape;

    body->CreateFixture(&fd);

    explosion_particles_[i] = body;
  }
}

void ktp::ProjectilePhysicsComponent::generateProjectileShape(B2Vec2Vector& shape, float size) {
  shape.push_back({0, -size - (size * 0.5f)}); // top
  shape.push_back({-size * 0.15f, -size}); // top left
  shape.push_back({-size * 0.15f,  size}); // down left
  shape.push_back({ size * 0.15f,  size}); // down right
  shape.push_back({ size * 0.15f, -size}); // top right
  shape.push_back(shape.front());          // top again
  shape.shrink_to_fit();
}

void ktp::ProjectilePhysicsComponent::transformRenderShape() {
  for (auto i = 0u; i < shape_.size(); ++i) {
    graphics_->render_shape_[i].x = ((shape_[i].x * SDL_cosf(body_->GetAngle()) - shape_[i].y * SDL_sinf(body_->GetAngle())) + body_->GetPosition().x) * kMetersToPixels;
    graphics_->render_shape_[i].y = ((shape_[i].x * SDL_sinf(body_->GetAngle()) + shape_[i].y * SDL_cosf(body_->GetAngle())) + body_->GetPosition().y) * kMetersToPixels;
  }
}

void ktp::ProjectilePhysicsComponent::update(const GameEntity& projectile, float delta_time) {
  if (detonated_) {
    if (Game::gameplay_timer_.getTicks() - kExplosionDuration_ > explosion_time_) {
      for (auto& bodies: explosion_particles_) {
        world_->DestroyBody(bodies);
      }
      owner_->deactivate();
    }
    transformRenderShape();
  } else {
    const auto threshold {size_};
    if (body_->GetPosition().x < -threshold || body_->GetPosition().x > b2_screen_size_.x + threshold ||
        body_->GetPosition().y < -threshold || body_->GetPosition().y > b2_screen_size_.y + threshold) {
      owner_->deactivate();
    } else {
      transformRenderShape();
      if (collided_) detonate();
    }
  }
}