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
  shader_program_ = Resources::getShader("particle");
  const float size {1000.f};
  vertices_data_ = {
    // vertices
    -0.5f * size,  0.5f * size,
     0.5f * size, -0.5f * size,
    -0.5f * size, -0.5f * size,
    -0.5f * size,  0.5f * size,
     0.5f * size, -0.5f * size,
     0.5f * size,  0.5f * size
  };
  vertices_.setup(vertices_data_);
  vao_.linkAttrib(vertices_, 0, 2, GL_FLOAT, 0, nullptr);
  // colors
  GLfloatVector colors_data_ {};
  colors_data_.resize((vertices_data_.size() / 2) * 3);
  for (auto& color_c: colors_data_) {
    color_c = generateRand(0.f, 1.f);
  }
  colors_.setup(colors_data_);
  vao_.linkAttrib(colors_, 1, 3, GL_FLOAT, 0, nullptr);
}

ktp::EmitterGraphicsComponent& ktp::EmitterGraphicsComponent::operator=(EmitterGraphicsComponent&& other) {
  if (this != &other) {
    blend_mode_            = other.blend_mode_;
    particles_pool_        = std::exchange(other.particles_pool_, nullptr);
    particles_pool_size_   = other.particles_pool_size_;
    alive_particles_count_ = other.alive_particles_count_;
    vao_                   = std::move(other.vao_);
    vertices_              = std::move(other.vertices_);
    colors_                = std::move(other.colors_);
    translations_          = std::move(other.translations_);
    vertices_data_         = std::move(other.vertices_data_);
    translations_data_     = std::move(other.translations_data_);
    shader_program_        = std::move(other.shader_program_);
    mvp_                   = std::move(other.mvp_);
  }
  return *this;
}

void ktp::EmitterGraphicsComponent::update(const GameEntity& emitter) {
  // ParticlesAtlas::particles_atlas.setBlendMode(blend_mode_);
  // for (auto i = 0u; i < particles_pool_size_; ++i) {
  //   if (particles_pool_[i].inUse()) {
  //     particles_pool_[i].draw();
  //   }
  // }
  shader_program_.use();
  vao_.bind();
  glDrawArraysInstanced(GL_TRIANGLES, 0, vertices_data_.size(), alive_particles_count_);
}

/* PHYSICS */

ktp::EmitterPhysicsComponent& ktp::EmitterPhysicsComponent::operator=(EmitterPhysicsComponent&& other) {
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
  delete[] graphics_->particles_pool_;
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
  // translations
  graphics_->translations_data_.resize(graphics_->particles_pool_size_);
  graphics_->translations_.setup(graphics_->translations_data_.data(), graphics_->particles_pool_size_ * sizeof(glm::vec2));
  graphics_->vao_.linkAttrib(graphics_->translations_, 2, 2, GL_FLOAT, 0, nullptr);

  // graphics_->translations_data_.resize(graphics_->particles_pool_size_ * 2);
  // graphics_->translations_.setup(graphics_->translations_data_);
  // graphics_->vao_.linkAttrib(graphics_->translations_, 2, 2, GL_FLOAT, 0, nullptr);
  glVertexAttribDivisor(2, 1);
}

void ktp::EmitterPhysicsComponent::update(const GameEntity& emitter, float delta_time) {
  for (auto i = 0u; i < graphics_->particles_pool_size_; ++i) {

    if (graphics_->particles_pool_[i].inUse() && graphics_->particles_pool_[i].update(graphics_->translations_data_[i])) {
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
  // translations_buffer_.setup(NULL, graphics_->particles_pool_size_ * 2 * sizeof(GLfloat), GL_STREAM_DRAW);
  // glBufferSubData(GL_ARRAY_BUFFER, 0, graphics_->particles_pool_size_ * 2 * sizeof(GLfloat), particules_positions_data_.data());
  // graphics_->vao_.linkAttrib(translations_buffer_, 2, 2, GL_FLOAT, 0, nullptr);

  graphics_->translations_.setup(graphics_->translations_data_.data(), graphics_->particles_pool_size_ * sizeof(glm::vec2));
  graphics_->vao_.linkAttrib(graphics_->translations_, 2, 2, GL_FLOAT, 0, nullptr);

  // graphics_->translations_.setup(graphics_->translations_data_);
  // graphics_->vao_.linkAttrib(graphics_->translations_, 2, 2, GL_FLOAT, 0, nullptr);
  glVertexAttribDivisor(2, 1);
  updateMVP();
}

void ktp::EmitterPhysicsComponent::updateMVP() {
  glm::mat4 model {1.f};
  model = glm::translate(model, glm::vec3(position_.x, position_.y, 0.f));
  // model = glm::rotate(model, angle_, glm::vec3(0.f, 0.f, 1.f));
  graphics_->mvp_ = camera_.projectionMatrix() * camera_.viewMatrix() * model;
  graphics_->shader_program_.use();
  graphics_->shader_program_.setMat4f("mvp", glm::value_ptr(graphics_->mvp_));
}
