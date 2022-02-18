#include "include/explosion.hpp"
#include "include/game.hpp"

// GRAPHICS

ktp::ExplosionGraphicsComponent::ExplosionGraphicsComponent() {
  translations_data_.resize(rays_);
  generateOpenGLStuff(ConfigParser::explosion_config.particle_radius_ * kMetersToPixels);
}

void ktp::ExplosionGraphicsComponent::generateOpenGLStuff(float size) {
  vertices_data_ = {
     0.00f * size,  0.50f * size, 0.f,  color_.r,  color_.g,  color_.b,   // top        0
    -0.33f * size, -0.50f * size, 0.f,  color_.r,  color_.g,  color_.b,   // left       1
     0.33f * size, -0.50f * size, 0.f,  color_.r,  color_.g,  color_.b    // right      2
  };
  vertices_.setup(vertices_data_);
  // vertices
  vao_.linkAttrib(vertices_, 0, 3, GL_FLOAT, 6 * sizeof(GLfloat), nullptr);
  // colors
  vao_.linkAttrib(vertices_, 1, 3, GL_FLOAT, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
  // translations, currently just resized
  translations_.setup(translations_data_.data(), translations_data_.size() * sizeof(glm::vec3));
  vao_.linkAttrib(translations_, 2, 3, GL_FLOAT, 0, nullptr);
  glVertexAttribDivisor(2, 1);
}

void ktp::ExplosionGraphicsComponent::update(const GameEntity& explosion) {
  shader_.use();
  vao_.bind();
  glDrawArraysInstanced(GL_TRIANGLES, 0, vertices_data_.size(), rays_);
}

// PHYSICS

ktp::ExplosionPhysicsComponent::ExplosionPhysicsComponent(GameEntity* owner, ExplosionGraphicsComponent* graphics):
 graphics_(graphics) {
  owner_ = owner;
  explosion_rays_.reserve(ConfigParser::projectiles_config.explosion_config_.rays_);
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
  // position_ = position;
  for (auto& body: explosion_rays_) {
    body->SetTransform(position, 0);
    body->SetEnabled(true);
  }
}

void ktp::ExplosionPhysicsComponent::update(const GameEntity& explosion, float delta_time) {
  if (detonated_ && Game::gameplay_timer_.milliseconds() - explosion_config_.duration_ > detonation_time_) {
    owner_->deactivate();
  } else {
    for (std::size_t i = 0; i < explosion_rays_.size(); ++i) {
      graphics_->translations_data_[i].x = explosion_rays_[i]->GetPosition().x * kMetersToPixels;
      graphics_->translations_data_[i].y = explosion_rays_[i]->GetPosition().y * kMetersToPixels;
    }
    graphics_->translations_.setup(graphics_->translations_data_.data(), graphics_->translations_data_.size() * sizeof(glm::vec3));
    updateMVP();
  }
}

void ktp::ExplosionPhysicsComponent::updateMVP() {
  glm::mat4 model {1.f};
  const glm::mat4 mvp {camera_.projectionMatrix() * camera_.viewMatrix() * model};
  graphics_->shader_.use();
  graphics_->shader_.setMat4f("mvp", glm::value_ptr(mvp));
}
