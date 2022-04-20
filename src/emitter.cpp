#include "include/camera.hpp"
#include "include/emitter.hpp"
#include "include/game_entity.hpp"
#include "include/random.hpp"
#include "sdl2_wrappers/sdl2_log.hpp"

/* GRAPHICS */

ktp::EmitterGraphicsComponent::EmitterGraphicsComponent() {
  constexpr float size {1.f};
  vertices_data_ = {                      // uv
     0.5f * size,  0.5f * size, 0.f,      1.f, 1.f, // top right      0
    -0.5f * size,  0.5f * size, 0.f,      0.f, 1.f, // top left       1
    -0.5f * size, -0.5f * size, 0.f,      0.f, 0.f, // down left      2
     0.5f * size, -0.5f * size, 0.f,      1.f, 0.f  // down right     3
  };
  vertices_.setup(vertices_data_);
  // vertices
  vao_.linkAttrib(vertices_, 0, 3, GL_FLOAT, 5 * sizeof(GLfloat), nullptr);
  // texture uv
  vao_.linkAttrib(vertices_, 1, 2, GL_FLOAT, 5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
  // EBO
  indices_.setup(indices_data_);
}

ktp::EmitterGraphicsComponent& ktp::EmitterGraphicsComponent::operator=(EmitterGraphicsComponent&& other) {
  if (this != &other) {
    blend_mode_          = other.blend_mode_;
    particles_pool_      = std::exchange(other.particles_pool_, nullptr);
    particles_pool_size_ = other.particles_pool_size_;
    vao_                 = std::move(other.vao_);
    vertices_            = std::move(other.vertices_);
    vertices_data_       = std::move(other.vertices_data_);
    indices_             = std::move(other.indices_);
    indices_data_        = std::move(other.indices_data_);
    subdata_             = std::move(other.subdata_);
    mvp_                 = std::move(other.mvp_);
    shader_              = std::move(other.shader_);
    texture_             = std::move(other.texture_);
  }
  return *this;
}

void ktp::EmitterGraphicsComponent::update(const GameEntity& emitter) {
  shader_.use();
  texture_.bind();
  vao_.bind();
  glDrawElementsInstanced(GL_TRIANGLES, static_cast<GLsizei>(indices_data_.size()), GL_UNSIGNED_INT, 0, particles_pool_size_);
}

/* PHYSICS */

ktp::EmitterPhysicsComponent& ktp::EmitterPhysicsComponent::operator=(EmitterPhysicsComponent&& other) {
  if (this != &other) {
    // inherited members
    collided_ = other.collided_;
    delta_    = std::move(other.delta_);
    owner_    = std::exchange(other.owner_, nullptr);
    size_     = other.size_;
    // own members
    angle_                 = other.angle_;
    alive_particles_count_ = other.alive_particles_count_;
    data_                  = std::exchange(other.data_, nullptr);
    first_available_       = std::exchange(other.first_available_, nullptr);
    graphics_              = std::exchange(other.graphics_, nullptr);
    interval_time_         = other.interval_time_;
    position_              = std::move(other.position_);
    start_time_            = other.start_time_;
    subdata_               = std::move(other.subdata_);
  }
  return *this;
}

void ktp::EmitterPhysicsComponent::generateParticles() {
  if (first_available_ == nullptr) {
    logWarn("NO FIRST AVAILABLE!!!!!");
    return;
  }

  const auto current_time {SDL2_Timer::SDL2Ticks()};
  if (current_time - start_time_ > data_->life_time_) return;
  if (current_time - interval_time_ < data_->emission_interval_.value_) return;

  const auto how_many {static_cast<unsigned int>(std::round(data_->emission_rate_.value_ * generateRand(data_->emission_rate_.rand_min_, data_->emission_rate_.rand_max_)))};
  for (auto i = 0u; i < how_many; ++i) {
    ParticleData new_data {};
    new_data.start_life_ = data_->max_particle_life_.value_ * generateRand(data_->max_particle_life_.rand_min_, data_->max_particle_life_.rand_max_);

    for (const auto& size: data_->sizes_) {
      const auto final_size {size.value_ * generateRand(size.rand_min_, size.rand_max_)};
      new_data.sizes_.push_back(final_size);
    }
    new_data.current_size_ = new_data.sizes_.front();

    new_data.colors_ = data_->colors_;
    new_data.current_color_ = data_->colors_.front();

    new_data.rotation_ = data_->rotation_.value_ * generateRand(data_->rotation_.rand_min_, data_->rotation_.rand_max_);

    new_data.start_rotation_speed_ = data_->start_rotation_speed_.value_ * generateRand(data_->start_rotation_speed_.rand_min_, data_->start_rotation_speed_.rand_max_);
    new_data.end_rotation_speed_ = data_->end_rotation_speed_.value_ * generateRand(data_->end_rotation_speed_.rand_min_, data_->end_rotation_speed_.rand_max_);

    const auto final_angle {generateRand(data_->angle_range_.min_, data_->angle_range_.max_) + angle_};
    const auto final_angle_cos {SDL_cosf(final_angle)};
    const auto final_angle_sin {SDL_sinf(final_angle)};

    for (const auto& speed: data_->speeds_) {
      const auto speed_multiplier {speed.value_ * generateRand(speed.rand_min_, speed.rand_max_)};
      const SDL_FPoint final_speed {
        (speed_multiplier * final_angle_cos) - (speed_multiplier * final_angle_sin),
        (speed_multiplier * final_angle_sin) + (speed_multiplier * final_angle_cos)
      };
      new_data.speeds_.push_back(final_speed);
    }
    new_data.current_speed_ = new_data.speeds_.front();

    new_data.position_ = position_;

    Particle* new_particle {first_available_};
    first_available_ = new_particle->getNext();
    new_particle->init(new_data);
    ++alive_particles_count_;

    if (first_available_ == nullptr) return;
  }
  interval_time_ = SDL2_Timer::SDL2Ticks();
}

void ktp::EmitterPhysicsComponent::inflatePool() {
  delete[] graphics_->particles_pool_;
  graphics_->particles_pool_ = new Particle[graphics_->particles_pool_size_];

  first_available_ = &graphics_->particles_pool_[0];

  for (auto i = 0u; i < graphics_->particles_pool_size_; ++i) {
    graphics_->particles_pool_[i].setNext(&graphics_->particles_pool_[i + 1]);
  }
  graphics_->particles_pool_[graphics_->particles_pool_size_ - 1].setNext(nullptr);
}

void ktp::EmitterPhysicsComponent::init(const std::string& type, const glm::vec3& pos) {
  setType(type);
  inflatePool();
  setupOpenGL();
  position_ = pos;
}

void ktp::EmitterPhysicsComponent::setType(const std::string& type) {
  bool emitter_found {false};
  for (const auto& emitter_type: ConfigParser::emitter_types) {
    if (emitter_type.type_ == type) {
      data_ = &emitter_type;
      graphics_->blend_mode_ = emitter_type.blend_mode_;
      graphics_->particles_pool_size_ = (emitter_type.max_particle_life_.value_ + 1u) * emitter_type.emission_rate_.value_ * 60;
      interval_time_ = emitter_type.emission_interval_.value_;
      emitter_found = true;
      break;
    }
  }
  if (!emitter_found) {
    logError("Emitter type \"" + type + "\" not found! Check emitters.xml for spelling errors or missing emitters.");
    return;
  }
}

void ktp::EmitterPhysicsComponent::setupOpenGL() {
  graphics_->vao_.bind();
  // subdata: translations(3), colors(4), size(1)
  subdata_.resize(graphics_->particles_pool_size_ * kComponents);
  graphics_->subdata_.setup(nullptr, subdata_.size() * sizeof(GLfloat), GL_STREAM_DRAW);
  // subdata translations
  graphics_->vao_.linkAttrib(graphics_->subdata_, 2, 3, GL_FLOAT, kComponents * sizeof(GLfloat), nullptr);
  glVertexAttribDivisor(2, 1);
  // subdata colors
  graphics_->vao_.linkAttrib(graphics_->subdata_, 3, 4, GL_FLOAT, kComponents * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
  glVertexAttribDivisor(3, 1);
  // subdata size
  graphics_->vao_.linkAttrib(graphics_->subdata_, 4, 1, GL_FLOAT, kComponents * sizeof(GLfloat), (void*)(7 * sizeof(GLfloat)));
  glVertexAttribDivisor(4, 1);
}

void ktp::EmitterPhysicsComponent::update(const GameEntity& emitter, float delta_time) {
  for (auto i = 0u; i < graphics_->particles_pool_size_; ++i) {
    if (graphics_->particles_pool_[i].inUse()) {
      // particle alive!
      if (graphics_->particles_pool_[i].update(delta_time, &subdata_[i * kComponents])) {
        // particle is no more, so we change the Z axis to 10
        subdata_[i * kComponents + 0] = 0.f;
        subdata_[i * kComponents + 1] = 0.f;
        subdata_[i * kComponents + 2] = 10.f;
        graphics_->particles_pool_[i].setNext(first_available_);
        first_available_ = &graphics_->particles_pool_[i];
        --alive_particles_count_;
      }
    }
  }
  graphics_->subdata_.setupSubData(subdata_.data(), subdata_.size() * sizeof(GLfloat));
  updateMVP();
}

void ktp::EmitterPhysicsComponent::updateMVP() {
  glm::mat4 model {1.f};
  const glm::mat4 mvp {camera_.projectionMatrix() * camera_.viewMatrix() * model};
  graphics_->shader_.use();
  graphics_->shader_.setMat4f("mvp", glm::value_ptr(mvp));
}
