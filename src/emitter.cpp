#include "include/camera.hpp"
#include "include/config_parser.hpp"
#include "include/emitter.hpp"
#include "include/game_entity.hpp"
#include "include/random.hpp"
#include "include/resources.hpp"
#include "sdl2_wrappers/sdl2_log.hpp"
#include "sdl2_wrappers/sdl2_renderer.hpp"
#include "sdl2_wrappers/sdl2_texture.hpp"

/* GRAPHICS */

ktp::EmitterGraphicsComponent::EmitterGraphicsComponent() {
  shader_ = Resources::getShader("particle");
}

void ktp::EmitterGraphicsComponent::update(const GameEntity& emitter) {
  // ParticlesAtlas::particles_atlas.setBlendMode(blend_mode_);
  // for (auto i = 0u; i < particles_pool_size_; ++i) {
  //   if (particles_pool_[i].inUse()) {
  //     particles_pool_[i].draw();
  //   }
  // }
  shader_.setMat4f("mvp", glm::value_ptr(mvp_));
  vao_.bind();
  glVertexAttribDivisor(0, 0); // particles vertices : always reuse the same 4 vertices -> 0
  glVertexAttribDivisor(1, 0); // colors
  glVertexAttribDivisor(2, 1); // colors
  glDrawArraysInstanced(GL_TRIANGLES, 0, 4, alive_particles_count_);
}

/* PHYSICS */

ktp::EmitterPhysicsComponent::EmitterPhysicsComponent(EmitterGraphicsComponent* graphics) noexcept: graphics_(graphics) {}

ktp::EmitterPhysicsComponent& ktp::EmitterPhysicsComponent::operator=(EmitterPhysicsComponent&& other) noexcept {
  if (this != &other) {
    // inherited members
    body_     = other.body_;
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
    position_              = other.position_;
    start_time_            = other.start_time_;

    vertex_buffer_ = std::move(other.vertex_buffer_);
    position_buffer_ = std::move(other.position_buffer_);
    // color_buffer_ = std::move(other.color_buffer_);
    particules_positions_data_ = std::move(other.particules_positions_data_);
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
    new_data.start_life_ = std::round(data_->max_particle_life_.value_ * generateRand(data_->max_particle_life_.rand_min_, data_->max_particle_life_.rand_max_));
    // we need to do this in order for the particle to be update()d at least one time,
    // so we avoid the "not first available" plague
    if (new_data.start_life_ <= 0u) new_data.start_life_ = 1u;

    new_data.texture_rect_ = data_->texture_rect_;

    for (const auto& size: data_->sizes_) {
      const auto final_size {size.value_ * generateRand(size.rand_min_, size.rand_max_)};
      new_data.sizes_.push_back(final_size);
    }
    new_data.current_size_ = new_data.sizes_.front();

    new_data.colors_ = data_->colors_;
    if (data_->colors_.size() == 0) {
      // no color specified in xml
      new_data.current_color_ = Colors::white;
    } else {
      new_data.current_color_ = data_->colors_.front();
    }

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
    graphics_->alive_particles_count_ = alive_particles_count_;

    if (first_available_ == nullptr) return;
  }
  interval_time_ = SDL2_Timer::SDL2Ticks();
}

void ktp::EmitterPhysicsComponent::inflatePool() {
  graphics_->particles_pool_ = new Particle[graphics_->particles_pool_size_];

  first_available_ = &graphics_->particles_pool_[0];

  for (auto i = 0u; i < graphics_->particles_pool_size_; ++i) {
    graphics_->particles_pool_[i].setNext(&graphics_->particles_pool_[i + 1]);
  }
  graphics_->particles_pool_[graphics_->particles_pool_size_ - 1].setNext(nullptr);
}

ktp::EmitterPhysicsComponent ktp::EmitterPhysicsComponent::makeEmitter(EmitterGraphicsComponent* graphics, const std::string& type, const SDL_FPoint& pos) {
  EmitterPhysicsComponent emitter {graphics};
  emitter.setType(type);
  emitter.setupOpenGL();
  emitter.setPosition(pos);
  emitter.particules_positions_data_.resize(emitter.graphics_->particles_pool_size_ * 2);
  return emitter;
}

void ktp::EmitterPhysicsComponent::setType(const std::string& type) {
  bool emitter_found {false};
  for (const auto& emitter_type: ConfigParser::emitter_types) {
    if (emitter_type.type_ == type) {
      data_ = &emitter_type;
      graphics_->blend_mode_ = emitter_type.blend_mode_;
      graphics_->particles_pool_size_ = (emitter_type.max_particle_life_.value_ + 1u) * emitter_type.emission_rate_.value_;
      interval_time_ = emitter_type.emission_interval_.value_;
      emitter_found = true;
      break;
    }
  }
  if (!emitter_found) {
    logError("Emitter type \"" + type + "\" not found! Check emitters.xml for spelling errors or missing emitters.");
    return;
  }
  inflatePool();
}

void ktp::EmitterPhysicsComponent::setupOpenGL() {
  graphics_->vao_.bind();
  const GLfloatVector vertex_buffer_data {
        // positions     // colors
    -0.05f,  0.05f,  1.0f, 0.0f, 0.0f,
     0.05f, -0.05f,  0.0f, 1.0f, 0.0f,
    -0.05f, -0.05f,  0.0f, 0.0f, 1.0f,

    -0.05f,  0.05f,  1.0f, 0.0f, 0.0f,
     0.05f, -0.05f,  0.0f, 1.0f, 0.0f,
     0.05f,  0.05f,  0.0f, 1.0f, 1.0f
  };
  vertex_buffer_.setup(vertex_buffer_data);
  graphics_->vao_.linkAttrib(vertex_buffer_, 0, 2, GL_FLOAT, 5 * sizeof(GLfloat), nullptr);
  graphics_->vao_.linkAttrib(vertex_buffer_, 1, 3, GL_FLOAT, 5 * sizeof(GLfloat), (void*)(2 * sizeof(GLfloat)));

  position_buffer_.setup(NULL, graphics_->particles_pool_size_ * 2 * sizeof(GLfloat), GL_STREAM_DRAW);
  graphics_->vao_.linkAttrib(position_buffer_, 2, 2, GL_FLOAT, 0, nullptr);
}

void ktp::EmitterPhysicsComponent::update(const GameEntity& emitter, float delta_time) {
  for (auto i = 0u; i < graphics_->particles_pool_size_; ++i) {

    if (graphics_->particles_pool_[i].inUse() && graphics_->particles_pool_[i].update(particules_positions_data_, i)) {
      graphics_->particles_pool_[i].setNext(first_available_);
      first_available_ = &graphics_->particles_pool_[i];
      --alive_particles_count_;
      graphics_->alive_particles_count_ = alive_particles_count_;
    }




    // if (data_->vortex_) {
    //   if (graphics_->particles_pool_[i].inUse() && graphics_->particles_pool_[i].update(Vortex{position_, data_->vortex_scale_, data_->vortex_speed_})) {
    //     graphics_->particles_pool_[i].setNext(first_available_);
    //     first_available_ = &graphics_->particles_pool_[i];
    //     --alive_particles_count_;
    //     graphics_->alive_particles_count_ = alive_particles_count_;
    //   }
    // } else { // no vortex
      // if (graphics_->particles_pool_[i].inUse() && graphics_->particles_pool_[i].update(particules_positions_data_, i)) {
      //   graphics_->particles_pool_[i].setNext(first_available_);
      //   first_available_ = &graphics_->particles_pool_[i];
      //   --alive_particles_count_;
      //   graphics_->alive_particles_count_ = alive_particles_count_;

        // position_buffer_.setup(nullptr, graphics_->particles_pool_size_ * 4 * sizeof(GLfloat), GL_STREAM_DRAW);
        // glBufferSubData(GL_ARRAY_BUFFER, 0, alive_particles_count_ * sizeof(GLfloat) * 4, particule_position_size_data_.data());
      // }
    //}
  }
  position_buffer_.setup(NULL, graphics_->particles_pool_size_ * 2 * sizeof(GLfloat), GL_STREAM_DRAW);
  glBufferSubData(GL_ARRAY_BUFFER, 0, graphics_->particles_pool_size_ * 2 * sizeof(GLfloat), particules_positions_data_.data());
  graphics_->vao_.linkAttrib(position_buffer_, 2, 2, GL_FLOAT, 0, nullptr);
  updateMVP();
}

void ktp::EmitterPhysicsComponent::updateMVP() {
  // glm::mat4 model {1.f};
  // model = glm::translate(model, glm::vec3(position_.x, position_.y, 0.f));
  // model = glm::rotate(model, angle_, glm::vec3(0.f, 0.f, 1.f));
  graphics_->mvp_ = camera_.projectionMatrix() * camera_.viewMatrix()/*  * model */;
}
