#include "include/explosion.hpp"
#include "include/game.hpp"

// GRAPHICS

void ktp::ExplosionGraphicsComponent::update(const GameEntity& explosion) {

}

// PHYSICS

ktp::ExplosionPhysicsComponent::ExplosionPhysicsComponent(GameEntity* owner, ExplosionGraphicsComponent* graphics):
 graphics_(graphics) {
  owner_ = owner;
  b2Body* current_body {nullptr};
  for (std::size_t i = 0; i < explosion_config_.rays_; ++i) {

    const float angle {(i / (float)explosion_config_.rays_) * 360.f * (b2_pi / 180.f)};
    const b2Vec2 ray_dir {SDL_sinf(angle), SDL_cosf(angle)};

    b2BodyDef bd;
    bd.bullet = true;
    bd.fixedRotation = true;
    bd.linearDamping = explosion_config_.linear_damping_;
    bd.linearVelocity = explosion_config_.blast_power_ * ray_dir;
    bd.type = b2_dynamicBody;
    bd.userData.pointer = reinterpret_cast<uintptr_t>(owner_);
    current_body = world_->CreateBody(&bd);

    b2CircleShape circle_shape;
    circle_shape.m_radius = explosion_config_.particle_radius_;

    b2FixtureDef fd;
    fd.density = explosion_config_.density_;
    fd.filter.groupIndex = -1;
    fd.friction = explosion_config_.friction_;
    fd.restitution = explosion_config_.restitution_;
    fd.shape = &circle_shape;

    current_body->CreateFixture(&fd);
    current_body->SetEnabled(false);
    explosion_rays_.push_back(current_body);
  }
}

ktp::ExplosionPhysicsComponent& ktp::ExplosionPhysicsComponent::operator=(ExplosionPhysicsComponent&& other) {
  if (this != &other) {
    // inherited members
    body_     = other.body_;
    collided_ = other.collided_;
    delta_    = std::move(other.delta_);
    owner_    = std::exchange(other.owner_, nullptr);
    size_     = other.size_;
    // own members
    detonated_        = other.detonated_;
    detonation_time_  = other.detonation_time_;
    explosion_config_ = std::move(other.explosion_config_);
    explosion_rays_   = std::move(other.explosion_rays_);
    graphics_         = std::exchange(other.graphics_, nullptr);
  }
  return *this;
}

void ktp::ExplosionPhysicsComponent::detonate(Uint32 time, const b2Vec2& position) {
  detonated_ = true;
  detonation_time_ = time;
  for (auto& body: explosion_rays_) {
    body->SetTransform(position, 0);
    body->SetEnabled(true);
  }
}

void ktp::ExplosionPhysicsComponent::update(const GameEntity& explosion, float delta_time) {
  if (detonated_ && Game::gameplay_timer_.milliseconds() - explosion_config_.duration_ > detonation_time_) {
    owner_->deactivate();
  } else {
    // const auto magic_num {explosion_config_.particle_radius_ * 1.5f};
    // graphics_->position_.x = body_->GetPosition().x * kMetersToPixels - magic_num;
    // graphics_->position_.y = body_->GetPosition().y * kMetersToPixels - magic_num;
  }
}
