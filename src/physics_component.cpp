#include "include/particle.hpp"
#include "include/physics_component.hpp"
#include "include/game_entity.hpp"
#include "include/graphics_component.hpp"
#include "include/input_component.hpp"
#include "include/box2d_scale.hpp"
#include "include/random.hpp"
#include <cmath> // std::round

SDL_FPoint ktp::PhysicsComponent::b2_screen_size_ {};
b2World* ktp::PhysicsComponent::world_ {nullptr};

void ktp::PhysicsComponent::setScreenSize(const SDL_FPoint& screen_size) {
  b2_screen_size_.x = screen_size.x / kMetersToPixels;
  b2_screen_size_.y = screen_size.y / kMetersToPixels;
}

/* AEROLITE */

ktp::AerolitePhysicsComponent::AerolitePhysicsComponent(AeroliteGraphicsComponent* graphics):
 graphics_(graphics) {
  size_ = kMaxSize_ * generateRand(0.3f, 1.f);
  generateAeroliteShape(shape_, size_);
  graphics_->render_shape_.resize(shape_.size());

  b2BodyDef body_def {};
  body_def.type = b2_dynamicBody;

  body_def.userData.pointer = reinterpret_cast<uintptr_t>(this);

  body_ = world_->CreateBody(&body_def);

  b2PolygonShape dynamic_box {};
  dynamic_box.SetAsBox(size_ * 0.5f, size_ * 0.5f);

  b2FixtureDef fixture_def {};
  fixture_def.shape = &dynamic_box;
  fixture_def.density = kKgPerMeter2_ / (size_ * size_);
  fixture_def.friction = 1.f;

  body_->CreateFixture(&fixture_def);

  body_->SetAngularVelocity(kMaxRotationSpeed_ * generateRand(-1.f, 1.f));
}

void ktp::AerolitePhysicsComponent::generateAeroliteShape(FPointsVector& shape, float size) {
  shape.push_back({-size * 0.5f, -size * 0.5f});
  shape.push_back({ size * 0.5f, -size * 0.5f});
  shape.push_back({ size * 0.5f,  size * 0.5f});
  shape.push_back({-size * 0.5f,  size * 0.5f});
  shape.push_back({-size * 0.5f, -size * 0.5f});
  shape.shrink_to_fit();
}

ktp::GameEntity ktp::AerolitePhysicsComponent::spawnAerolite() {
  GameEntity aerolite {GameEntities::Aerolite};
  static int side {};
  const float delta {kMaxSpeed_ * generateRand(0.1f, 1.f)};
  switch (side) {
    case 0: // up
      aerolite.physics_->getBody()->SetTransform({b2_screen_size_.x * 0.5f, 0.f}, aerolite.physics_->getBody()->GetAngle());
      aerolite.physics_->getBody()->SetLinearVelocity({0, delta});
      ++side;
      break;
    case 1: // right
      aerolite.physics_->getBody()->SetTransform({b2_screen_size_.x, b2_screen_size_.y * 0.5f}, aerolite.physics_->getBody()->GetAngle());
      aerolite.physics_->getBody()->SetLinearVelocity({-delta, 0});
      ++side;
      break;
    case 2: // down
      aerolite.physics_->getBody()->SetTransform({b2_screen_size_.x * 0.5f, b2_screen_size_.y}, aerolite.physics_->getBody()->GetAngle());
      aerolite.physics_->getBody()->SetLinearVelocity({0, -delta});
      ++side;
      break;
    case 3: // left
      aerolite.physics_->getBody()->SetTransform({0.f, b2_screen_size_.y * 0.5f}, aerolite.physics_->getBody()->GetAngle());
      aerolite.physics_->getBody()->SetLinearVelocity({delta, 0});
      side = 0;
      break;
  }
  return aerolite;
}

void ktp::AerolitePhysicsComponent::transformRenderShape() {
  for (auto i = 0u; i < shape_.size(); ++i) {
    graphics_->render_shape_[i].x = ((shape_[i].x * SDL_cosf(body_->GetAngle()) - shape_[i].y * SDL_sinf(body_->GetAngle())) + body_->GetPosition().x) * kMetersToPixels;
    graphics_->render_shape_[i].y = ((shape_[i].x * SDL_sinf(body_->GetAngle()) + shape_[i].y * SDL_cosf(body_->GetAngle())) + body_->GetPosition().y) * kMetersToPixels;
  }
}

void ktp::AerolitePhysicsComponent::update(const GameEntity& aerolite, float delta_time) {
  transformRenderShape();

  aabb_ = body_->GetFixtureList()->GetAABB(0);

  constexpr auto threshold {0.1f};
  if (aabb_.upperBound.x < 0 || aabb_.lowerBound.x > b2_screen_size_.x + threshold
   || aabb_.upperBound.y < 0 || aabb_.lowerBound.y > b2_screen_size_.y + threshold) {
    to_be_deleted_ = true;
  }
  // this will be usefull when the body_ has n fixtures
  /*
  aabb_.lowerBound = b2Vec2( FLT_MAX, FLT_MAX);
  aabb_.upperBound = b2Vec2(-FLT_MAX,-FLT_MAX);
  for (b2Fixture* fixture = body_->GetFixtureList(); fixture; fixture = fixture->GetNext()) {
    aabb_.Combine(aabb_, fixture->GetAABB(0)); /// wtf is childIndex
  } */
}

/* EMITTER */

struct ktp::Vortex {
  SDL_FPoint position_{};
  float scale_{};
  float speed_{};
};

void ktp::EmitterPhysicsComponent::generateParticles() {
  if (first_available_ == nullptr) {
    logMessage("WARNING!! NO FIRST AVAILABLE!!!!!");
    return;
  }

  const auto current_time{SDL2_Timer::getSDL2Ticks()};
  if (current_time - start_time_ > data_->life_time_) return;
  if (current_time - interval_time_ < data_->emission_interval_.value_) return;

  const auto how_many{static_cast<unsigned int>(std::round(data_->emission_rate_.value_ * generateRand(data_->emission_rate_.rand_min_, data_->emission_rate_.rand_max_)))};
  for (auto i = 0u; i < how_many; ++i) {
    ParticleData new_data{};
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
      new_data.current_color_ = ktp::Colors::white;
    } else {
      new_data.current_color_ = data_->colors_.front();
    }

    new_data.rotation_ = data_->rotation_.value_ * generateRand(data_->rotation_.rand_min_, data_->rotation_.rand_max_);

    new_data.start_rotation_speed_ = data_->start_rotation_speed_.value_ * generateRand(data_->start_rotation_speed_.rand_min_, data_->start_rotation_speed_.rand_max_);
    new_data.end_rotation_speed_ = data_->end_rotation_speed_.value_ * generateRand(data_->end_rotation_speed_.rand_min_, data_->end_rotation_speed_.rand_max_);

    const auto final_angle {generateRand(data_->angle_range_.min_, data_->angle_range_.max_)};
    const auto final_angle_cosf {SDL_cosf(final_angle)};
    const auto final_angle_sinf {SDL_sinf(final_angle)};

    for (const auto& speed: data_->speeds_) {
      const auto speed_multiplier {speed.value_ * generateRand(speed.rand_min_, speed.rand_max_)};
      const SDL_FPoint final_speed {
        (speed_multiplier * final_angle_cosf) - (speed_multiplier * final_angle_sinf),
        (speed_multiplier * final_angle_sinf) + (speed_multiplier * final_angle_cosf)
      };
      new_data.speeds_.push_back(final_speed);
    }
    new_data.current_speed_ = new_data.speeds_.front();

    new_data.position_ = position_;

    Particle* new_particle{first_available_};
    first_available_ = new_particle->getNext();
    new_particle->init(new_data);
    ++alive_particles_count_;

    if (first_available_ == nullptr) return;
  }
  interval_time_ = SDL2_Timer::getSDL2Ticks();
}

void ktp::EmitterPhysicsComponent::inflatePool() {
  graphics_->particles_pool_ = new Particle[graphics_->particles_pool_size_];

  first_available_ = &graphics_->particles_pool_[0];

  for (auto i = 0u; i < graphics_->particles_pool_size_; ++i) {
    graphics_->particles_pool_[i].setNext(&graphics_->particles_pool_[i + 1]);
  }
  graphics_->particles_pool_[graphics_->particles_pool_size_ - 1].setNext(nullptr);
}

ktp::GameEntity ktp::EmitterPhysicsComponent::makeEmitter(const std::string& type, const SDL_FPoint& pos) {
  GameEntity emitter {GameEntities::Emitter};
  setType(static_cast<EmitterPhysicsComponent*>(emitter.physics_.get()), type);
  setPosition(static_cast<EmitterPhysicsComponent*>(emitter.physics_.get()), pos);
  return emitter;
}

void ktp::EmitterPhysicsComponent::setType(const std::string& type) {
  bool emitter_found {false};
  for (const auto& emitter_type: EmitterParser::emitter_types) {
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
    logErrorMessage("Emitter type \"" + type + "\" not found! Check emitters.xml for spelling errors or missing emitters.");
    return;
  }
  inflatePool();
}

void ktp::EmitterPhysicsComponent::setType(EmitterPhysicsComponent* physics, const std::string& type) {
  bool emitter_found {false};
  for (const auto& emitter_type: EmitterParser::emitter_types) {
    if (emitter_type.type_ == type) {
      physics->data_ = &emitter_type;
      physics->graphics_->blend_mode_ = emitter_type.blend_mode_;
      physics->graphics_->particles_pool_size_ = (emitter_type.max_particle_life_.value_ + 1u) * emitter_type.emission_rate_.value_;
      physics->interval_time_ = emitter_type.emission_interval_.value_;
      emitter_found = true;
      break;
    }
  }
  if (!emitter_found) {
    logErrorMessage("Emitter type \"" + type + "\" not found! Check emitters.xml for spelling errors or missing emitters.");
    return;
  }
  physics->inflatePool();
}

void ktp::EmitterPhysicsComponent::update(const GameEntity& emitter, float delta_time) {
  for (auto i = 0u; i < graphics_->particles_pool_size_; ++i) {
    if (data_->vortex_) {
      if (graphics_->particles_pool_[i].inUse() && graphics_->particles_pool_[i].update(Vortex{position_, data_->vortex_scale_, data_->vortex_speed_})) {
        graphics_->particles_pool_[i].setNext(first_available_);
        first_available_ = &graphics_->particles_pool_[i];
        --alive_particles_count_;
      }
    } else { // no vortex
      if (graphics_->particles_pool_[i].inUse() && graphics_->particles_pool_[i].update()) {
        graphics_->particles_pool_[i].setNext(first_available_);
        first_available_ = &graphics_->particles_pool_[i];
        --alive_particles_count_;
      }
    }
  }
}

/* PLAYER */

ktp::PlayerPhysicsComponent::PlayerPhysicsComponent(PlayerGraphicsComponent* graphics):
 graphics_(graphics) {
  size_ = kDefaultPlayerSize_;
  generatePlayerShape(shape_, flame_shape_, size_);
  graphics_->render_shape_.resize(shape_.size());
  graphics_->render_flame_shape_.resize(flame_shape_.size());
  setBox2D();
  // flame_emitter_ = std::make_unique<GameEntity>(EmitterPhysicsComponent::makeEmitter("fire", {body_->GetPosition().x, body_->GetPosition().y}));
}

void ktp::PlayerPhysicsComponent::checkWrap() {
  const auto threshold {size_ * 0.5f};
  if (body_->GetPosition().x < -threshold) {
    body_->SetTransform({b2_screen_size_.x + threshold, body_->GetPosition().y}, body_->GetAngle());
  } else if (body_->GetPosition().x > b2_screen_size_.x + threshold) {
    body_->SetTransform({-threshold, body_->GetPosition().y}, body_->GetAngle());
  }
  if (body_->GetPosition().y < -threshold) {
    body_->SetTransform({body_->GetPosition().x, b2_screen_size_.y + threshold}, body_->GetAngle());
  } else if (body_->GetPosition().y > b2_screen_size_.y + threshold) {
    body_->SetTransform({body_->GetPosition().x, -threshold}, body_->GetAngle());
  }
}

void ktp::PlayerPhysicsComponent::generatePlayerShape(FPointsVector& shape, FPointsVector& flame_shape, float size) {
  shape.push_back({          0.f, -size * 0.50f}); // top
  shape.push_back({-size * 0.33f,  size * 0.50f}); // left
  shape.push_back({-size * 0.15f,  size * 0.33f}); // left flap
  shape.push_back({ size * 0.15f,  size * 0.33f}); // right flap
  shape.push_back({ size * 0.33f,  size * 0.50f}); // right
  shape.push_back({          0.f, -size * 0.50f}); // top again
  shape.shrink_to_fit();

  flame_shape.push_back({          0.f, size * 0.40f}); // bottom           2 ____ 1
  flame_shape.push_back({ size * 0.15f, size * 0.35f}); // right vertice      \  /
  flame_shape.push_back({-size * 0.15f, size * 0.35f}); // left vertice        \/
  flame_shape.push_back({          0.f, size * 0.40f}); // bottom again        3/0
  flame_shape.shrink_to_fit();
}

void ktp::PlayerPhysicsComponent::setBox2D() {
  // Player
  b2BodyDef body_def {};
  body_def.type = b2_dynamicBody;
  body_def.bullet = true;
  body_def.position.Set(b2_screen_size_.x * 0.5f, b2_screen_size_.y * 0.5f);
  body_def.userData.pointer = reinterpret_cast<uintptr_t>(this);

  body_ = world_->CreateBody(&body_def);
  // triangle CCW
  b2Vec2 vertices[3];
  vertices[0].Set(           0.f, -size_ * 0.50f); // top
  vertices[1].Set(-size_ * 0.33f,  size_ * 0.50f); // left
  vertices[2].Set( size_ * 0.33f,  size_ * 0.50f); // right
  b2PolygonShape triangle {};
  triangle.Set(vertices, 3);

  b2FixtureDef fixture_def {};
  fixture_def.shape = &triangle;
  fixture_def.density = 1.5f;
  fixture_def.friction = 0.8f;

  body_->CreateFixture(&fixture_def);
}

void ktp::PlayerPhysicsComponent::transformRenderShape() {
  for (auto i = 0u; i < shape_.size(); ++i) {
    graphics_->render_shape_[i].x = ((shape_[i].x * SDL_cosf(body_->GetAngle()) - shape_[i].y * SDL_sinf(body_->GetAngle())) + body_->GetPosition().x) * kMetersToPixels;
    graphics_->render_shape_[i].y = ((shape_[i].x * SDL_sinf(body_->GetAngle()) + shape_[i].y * SDL_cosf(body_->GetAngle())) + body_->GetPosition().y) * kMetersToPixels;
  }
  if (thrusting_) {
    for (auto i = 0u; i < flame_shape_.size(); ++i) {
      graphics_->render_flame_shape_[i].x = ((flame_shape_[i].x * SDL_cosf(body_->GetAngle()) - flame_shape_[i].y * SDL_sinf(body_->GetAngle())) + body_->GetPosition().x) * kMetersToPixels;
      graphics_->render_flame_shape_[i].y = ((flame_shape_[i].x * SDL_sinf(body_->GetAngle()) + flame_shape_[i].y * SDL_cosf(body_->GetAngle())) + body_->GetPosition().y) * kMetersToPixels;
    }
  }
}

void ktp::PlayerPhysicsComponent::update(const GameEntity& player, float delta_time) {
  graphics_->thrusting_ = thrusting_;
  checkWrap();
  transformRenderShape();
  // flame_emitter_->physics_->setPosition({body_->GetPosition().x * kMetersToPixels, body_->GetPosition().y * kMetersToPixels});
  // flame_emitter_->update(delta_time);
}

/* PROJECTILE */

ktp::ProjectilePhysicsComponent::ProjectilePhysicsComponent(ProjectileGraphicsComponent* graphics):
 graphics_(graphics) {
  size_ = kDefaultProjectileSize_;
  generateProjectileShape(shape_, size_);
  graphics_->render_shape_.resize(shape_.size());

  b2BodyDef body_def {};
  body_def.type = b2_dynamicBody;
  body_def.bullet = true;
  body_def.userData.pointer = reinterpret_cast<uintptr_t>(this);

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

void ktp::ProjectilePhysicsComponent::generateProjectileShape(FPointsVector& shape, float size) {
  shape.push_back({-size * 0.15f, -size}); // top left
  shape.push_back({-size * 0.15f,  size}); // down left
  shape.push_back({ size * 0.15f,  size}); // down right
  shape.push_back({ size * 0.15f, -size}); // top right
  shape.push_back({-size * 0.15f, -size}); // top left again
  shape.shrink_to_fit();
}

void ktp::ProjectilePhysicsComponent::transformRenderShape() {
  for (auto i = 0u; i < shape_.size(); ++i) {
    graphics_->render_shape_[i].x = ((shape_[i].x * SDL_cosf(body_->GetAngle()) - shape_[i].y * SDL_sinf(body_->GetAngle())) + body_->GetPosition().x) * kMetersToPixels;
    graphics_->render_shape_[i].y = ((shape_[i].x * SDL_sinf(body_->GetAngle()) + shape_[i].y * SDL_cosf(body_->GetAngle())) + body_->GetPosition().y) * kMetersToPixels;
  }
}

void ktp::ProjectilePhysicsComponent::update(const GameEntity& projectile, float delta_time) {
  const auto threshold {size_};
  // check if laser is out of screen
  if (body_->GetPosition().x < -threshold || body_->GetPosition().x > b2_screen_size_.x + threshold ||
      body_->GetPosition().y < -threshold || body_->GetPosition().y > b2_screen_size_.y + threshold) {
    to_be_deleted_ = true;
  } else {
    transformRenderShape();
  }
}