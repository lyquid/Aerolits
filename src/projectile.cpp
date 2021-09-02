#include "include/box2d_scale.hpp"
#include "include/emitter.hpp"
#include "include/game.hpp"
#include "include/game_entity.hpp"
#include "include/projectile.hpp"
#include "../sdl2_wrappers/sdl2_renderer.hpp"
#include "../sdl2_wrappers/sdl2_timer.hpp"
#include <box2d/box2d.h>

/* GRAPHICS */

ktp::ProjectileGraphicsComponent::ProjectileGraphicsComponent() noexcept {
  exhaust_emitter_ = std::make_unique<EmitterGraphicsComponent>();
}

void ktp::ProjectileGraphicsComponent::update(const GameEntity& projectile, const SDL2_Renderer& renderer) {
  renderer.setDrawColor(color_);
  renderer.drawLines(render_shape_);
  exhaust_emitter_->update(projectile, renderer);
}

/* PHYSICS */

ktp::ProjectilePhysicsComponent::ProjectilePhysicsComponent(GameEntity* owner, ProjectileGraphicsComponent* graphics) noexcept:
 graphics_(graphics) {
  owner_ = owner;
  size_ = ConfigParser::projectiles_config.size_;
  generateProjectileShape(shape_, size_);
  graphics_->render_shape_.resize(shape_.size());

  explosion_config_ = ConfigParser::projectiles_config.explosion_config_;

  b2BodyDef body_def {};
  body_def.type = b2_dynamicBody;
  body_def.bullet = true;
  body_def.userData.pointer = reinterpret_cast<uintptr_t>(owner_);

  body_ = world_->CreateBody(&body_def);

  b2Vec2 laser_vertices[5];
  laser_vertices[0].Set(0, -size_ - (size_ * 0.5f)); // top
  laser_vertices[1].Set(-size_ * 0.15f, -size_); // top left
  laser_vertices[2].Set(-size_ * 0.15f,  size_); // down left
  laser_vertices[3].Set( size_ * 0.15f,  size_); // down right
  laser_vertices[4].Set( size_ * 0.15f, -size_); // up right

  b2PolygonShape projectile_shape {};
  projectile_shape.Set(laser_vertices, 5);

  b2FixtureDef projectile_fixture_def {};
  projectile_fixture_def.shape = &projectile_shape;
  projectile_fixture_def.density = ConfigParser::projectiles_config.density_;
  projectile_fixture_def.friction = ConfigParser::projectiles_config.friction_;
  projectile_fixture_def.restitution = ConfigParser::projectiles_config.restitution_;

  body_->CreateFixture(&projectile_fixture_def);

  exhaust_emitter_ = std::make_unique<EmitterPhysicsComponent>(EmitterPhysicsComponent::makeEmitter(graphics_->exhaust_emitter_.get(), "projectile_exhaust", {body_->GetPosition().x, body_->GetPosition().y}));

  fired_time_ = Game::gameplay_timer_.milliseconds();
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
    armed_            = other.armed_;
    arm_time_         = other.arm_time_;
    cos_              = other.cos_;
    detonated_        = other.detonated_;
    exhaust_emitter_  = std::move(other.exhaust_emitter_);
    explosion_config_ = std::move(other.explosion_config_);
    fired_time_       = other.fired_time_;
    graphics_         = std::exchange(other.graphics_, nullptr);
    sin_              = other.sin_;
    speed_            = other.speed_;
  }
  return *this;
}

void ktp::ProjectilePhysicsComponent::detonate() {
  detonated_ = true;
  for (std::size_t i = 0; i < explosion_config_.rays_; ++i) {

    const float angle = (i / (float)explosion_config_.rays_) * 360 * (b2_pi / 180);
    const b2Vec2 ray_dir {SDL_sinf(angle), SDL_cosf(angle)};

    const auto xphysics {static_cast<XParticlePhysicsComponent*>(GameEntity::createEntity(EntityTypes::ExplosionParticle)->physics())};
    if (!xphysics) return;

    xphysics->setDuration(explosion_config_.duration_);
    xphysics->setDetonationTime(Game::gameplay_timer_.milliseconds());
    xphysics->setRadius(explosion_config_.particle_radius_ * kMetersToPixels);

    b2BodyDef bd;
    bd.bullet = true;
    bd.fixedRotation = true;
    bd.linearDamping = explosion_config_.linear_damping_;
    bd.linearVelocity = explosion_config_.blast_power_ * ray_dir;
    bd.position = body_->GetPosition();
    bd.type = b2_dynamicBody;
    bd.userData.pointer = reinterpret_cast<uintptr_t>(xphysics->owner());
    xphysics->setBody(world_->CreateBody(&bd));

    b2CircleShape circle_shape;
    circle_shape.m_radius = explosion_config_.particle_radius_;

    b2FixtureDef fd;
    fd.density = explosion_config_.density_;
    fd.filter.groupIndex = -1;
    fd.friction = explosion_config_.friction_;
    fd.restitution = explosion_config_.restitution_;
    fd.shape = &circle_shape;

    xphysics->body()->CreateFixture(&fd);

    xphysics->graphics_->position_.x = body_->GetPosition().x * kMetersToPixels;
    xphysics->graphics_->position_.y = body_->GetPosition().y * kMetersToPixels;
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
    graphics_->render_shape_[i].x = ((shape_[i].x * cos_ - shape_[i].y * sin_) + body_->GetPosition().x) * kMetersToPixels;
    graphics_->render_shape_[i].y = ((shape_[i].x * sin_ + shape_[i].y * cos_) + body_->GetPosition().y) * kMetersToPixels;
  }
}

void ktp::ProjectilePhysicsComponent::update(const GameEntity& projectile, float delta_time) {
  if (collided_) {
    if (armed_) {
      detonate();
      owner_->deactivate();
      return;
    } else {
      collided_ = false;
    }
  }
  const auto threshold {size_};
  if (body_->GetPosition().x < -threshold || body_->GetPosition().x > b2_screen_size_.x + threshold ||
      body_->GetPosition().y < -threshold || body_->GetPosition().y > b2_screen_size_.y + threshold) {
    owner_->deactivate();
    return;
  }

  cos_ = SDL_cosf(body_->GetAngle());
  sin_ = SDL_sinf(body_->GetAngle());

  if (Game::gameplay_timer_.milliseconds() - fired_time_ > arm_time_) {
    armed_ = true;
    delta_.x +=  sin_ * speed_ * delta_time;
    delta_.y += -cos_ * speed_ * delta_time;
    body_->ApplyLinearImpulseToCenter({delta_.x, delta_.y}, true);
  }

  transformRenderShape();

  exhaust_emitter_->setPosition({
    (body_->GetPosition().x * kMetersToPixels) - size_ * kMetersToPixels * sin_,
    (body_->GetPosition().y * kMetersToPixels) + size_ * kMetersToPixels * cos_
  });
  exhaust_emitter_->update(projectile, delta_time);
  if (armed_) exhaust_emitter_->generateParticles();
}