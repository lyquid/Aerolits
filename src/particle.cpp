#include "particle.hpp"
#include "../include/resources_path.hpp"
#include <string>

ktp::SDL2_Texture ktp::ParticlesAtlas::particles_atlas{};

void ktp::ParticlesAtlas::loadTexture(SDL2_Renderer& ren) {
  particles_atlas.setRenderer(ren);
  const std::string file{"particles.png"};
  const std::string path{getResourcesPath("textures") + file};
  particles_atlas.loadFromFile(path);
}

void ktp::Particle::draw() const {
  ParticlesAtlas::particles_atlas.setBlendMode(state_.live_.blend_mode_);
  ParticlesAtlas::particles_atlas.setColorMod(state_.live_.current_color_);
  ParticlesAtlas::particles_atlas.setAlphaMod(state_.live_.current_color_.a);
  ParticlesAtlas::particles_atlas.render(state_.live_.texture_rect_, 
                                        {static_cast<int>(state_.live_.position_.x), 
                                         static_cast<int>(state_.live_.position_.y), 
                                         static_cast<int>(state_.live_.current_size_), 
                                         static_cast<int>(state_.live_.current_size_)},
                                        state_.live_.rotation_);
}

void ktp::Particle::init(const ParticleData& data) {
  state_.live_.texture_rect_ = data.texture_rect_;
  
  life_ = data.start_life_;
  state_.live_.start_life_ = data.start_life_;

  state_.live_.blend_mode_ = data.blend_mode_;

  state_.live_.start_size_ = data.start_size_;
  state_.live_.current_size_ = data.start_size_;
  state_.live_.end_size_ = data.end_size_;

  state_.live_.start_color_ = data.start_color_;
  state_.live_.current_color_ = data.start_color_;
  state_.live_.end_color_ = data.end_color_;

  state_.live_.rotation_ = data.rotation_;

  state_.live_.start_rotation_speed_ = data.start_rotation_speed_;
  state_.live_.current_rotation_speed_ = data.start_rotation_speed_;
  state_.live_.end_rotation_speed_ = data.end_rotation_speed_;

  state_.live_.start_speed_ = data.start_speed_;
  state_.live_.current_speed_ = data.start_speed_;
  state_.live_.end_speed_ = data.end_speed_;





  // position is set to center the texture on the emitter
  state_.live_.position_ = {data.position_.x - data.start_size_ * 0.5f, 
                            data.position_.y - data.start_size_ * 0.5f};
  state_.live_.time_step_ = 0.f;
}

SDL_Color ktp::Particle::interpolateColors(const SDL_Color& start_color, float time_step, const SDL_Color& end_color) {
  return {
    interpolateRange(start_color.r, time_step, end_color.r),
    interpolateRange(start_color.g, time_step, end_color.g),
    interpolateRange(start_color.b, time_step, end_color.b),
    interpolateRange(start_color.a, time_step, end_color.a)
  };
}

/* return true if the previously live particle gave up the ghost in that frame */
bool ktp::Particle::update(float delta_time) {
  // time step increment to interpolate
  state_.live_.time_step_ += (1.f / state_.live_.start_life_);
  if (state_.live_.time_step_ >= 1.f) state_.live_.time_step_ = 0.f; 
  // size interpolation
  const float last_size{state_.live_.current_size_};
  state_.live_.current_size_ = interpolateRange(state_.live_.start_size_, state_.live_.time_step_, state_.live_.end_size_);
  // new position based on the new size
  state_.live_.position_.x += ((last_size - state_.live_.current_size_) * 0.5f);
  state_.live_.position_.y += ((last_size - state_.live_.current_size_) * 0.5f);
  // color interpolation
  state_.live_.current_color_ = interpolateColors(state_.live_.start_color_, state_.live_.time_step_, state_.live_.end_color_);
  // rotation speed interpolation
  state_.live_.current_rotation_speed_ = interpolateRange(state_.live_.start_rotation_speed_, state_.live_.time_step_, state_.live_.end_rotation_speed_);
  state_.live_.rotation_ += state_.live_.current_rotation_speed_;
  // speed interpolation
  state_.live_.current_speed_.x = interpolateRange(state_.live_.start_speed_.x, state_.live_.time_step_, state_.live_.end_speed_.x);
  state_.live_.current_speed_.y = interpolateRange(state_.live_.start_speed_.y, state_.live_.time_step_, state_.live_.end_speed_.y);
  state_.live_.position_.x += state_.live_.current_speed_.x;
  state_.live_.position_.y += state_.live_.current_speed_.y;


  --life_;
  return life_ == 0; 
}