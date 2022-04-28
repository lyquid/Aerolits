#include "include/explosion.hpp"
#include "include/game.hpp" // gameplay timer
#include "include/game_entity.hpp"

// GRAPHICS

ktp::ExplosionGraphicsComponent::ExplosionGraphicsComponent() {
  generateOpenGLStuff(ConfigParser::explosion_config.particle_radius_ * kMetersToPixels);
}

void ktp::ExplosionGraphicsComponent::generateOpenGLStuff(float size) {
  vertices_data_ = {
     0.5f * size,  0.5f * size, 0.f,  color_.r,  color_.g,  color_.b,  1.f, 1.f, // top right      0
    -0.5f * size,  0.5f * size, 0.f,  color_.r,  color_.g,  color_.b,  0.f, 1.f, // top left       1
    -0.5f * size, -0.5f * size, 0.f,  color_.r,  color_.g,  color_.b,  0.f, 0.f, // down left      2
     0.5f * size, -0.5f * size, 0.f,  color_.r,  color_.g,  color_.b,  1.f, 0.f  // down right     3
  };
  vertices_.setup(vertices_data_);
  // vertices
  vao_.linkAttrib(vertices_, 0, 3, GL_FLOAT, 8 * sizeof(GLfloat), nullptr);
  // colors
  vao_.linkAttrib(vertices_, 1, 3, GL_FLOAT, 8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
  // texture
  vao_.linkAttrib(vertices_, 2, 2, GL_FLOAT, 8 * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat)));
  // EBO
  indices_data_ = { 0, 1, 2, 0, 2, 3 };
  indices_.setup(indices_data_);
  // translations
  translations_.setup(nullptr, rays_ * sizeof(glm::vec3), GL_STREAM_DRAW);
  vao_.linkAttrib(translations_, 3, 3, GL_FLOAT, 0, nullptr);
  glVertexAttribDivisor(3, 1);
}

void ktp::ExplosionGraphicsComponent::update(const GameEntity& explosion) {
  if (render_) {
    shader_.use();
    texture_.bind();
    vao_.bind();
    glDrawElementsInstanced(GL_TRIANGLES, static_cast<GLsizei>(indices_data_.size()), GL_UNSIGNED_INT, 0, rays_);
  }
}

// PHYSICS

ktp::ExplosionPhysicsComponent::ExplosionPhysicsComponent(GameEntity* owner, ExplosionGraphicsComponent* graphics):
 graphics_(graphics) {
  owner_ = owner;
  explosion_rays_.reserve(explosion_config_.rays_);
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

    translations_data_.resize(explosion_config_.rays_);
  }
}

ktp::ExplosionPhysicsComponent& ktp::ExplosionPhysicsComponent::operator=(ExplosionPhysicsComponent&& other) {
  if (this != &other) {
    // inherited members
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
  if (detonated_) {
    if (Game::gameplay_timer_.milliseconds() - explosion_config_.duration_ > detonation_time_) {
      owner_->deactivate();
      return;
    } else {
      graphics_->render_ = true;
      for (std::size_t i = 0; i < explosion_rays_.size(); ++i) {
        translations_data_[i].x = explosion_rays_[i]->GetPosition().x * kMetersToPixels;
        translations_data_[i].y = explosion_rays_[i]->GetPosition().y * kMetersToPixels;
      }
      graphics_->translations_.setupSubData(translations_data_.data(), translations_data_.size() * sizeof(glm::vec3));
      updateMVP();
    }
  }
}

void ktp::ExplosionPhysicsComponent::updateMVP() {
  glm::mat4 model {1.f};
  const glm::mat4 mvp {camera_.projectionMatrix() * camera_.viewMatrix() * model};
  graphics_->shader_.use();
  graphics_->shader_.setMat4f("mvp", glm::value_ptr(mvp));
}
