#include "particle_system.hpp"

/* XML PARSER namespace */

std::vector<ktp::EmitterType> ktp::XMLParser::emitter_types{};

void ktp::XMLParser::constructEmitterTypesVector(const pugi::xml_document& doc) {
  const auto emitters = doc.child("emitterTypes");
  std::string types_created{};
  for (const auto& emitter: emitters) {
    EmitterType emi;
    
    const std::string type{emitter.attribute("type").as_string()};
    types_created += (type + ' ');
    if (type == std::string{"fire"}) {
      emi.type_ = EmitterTypes::Fire;
    } else if (type == std::string{"smoke"}) {
      emi.type_ = EmitterTypes::Smoke;
    } else {
      logErrorMessage("Unknown emitter type");
      return;
    }
    emi.angle_range_.min_ = emitter.child("angleRange").attribute("min").as_float();
    emi.angle_range_.max_ = emitter.child("angleRange").attribute("max").as_float();

    emi.rotation_speed_.value_    = emitter.child("rotSpeed").attribute("value").as_float();
    emi.rotation_speed_.rand_min_ = emitter.child("rotSpeed").attribute("randMin").as_float();
    emi.rotation_speed_.rand_max_ = emitter.child("rotSpeed").attribute("randMax").as_float();

    emi.start_speed_.value_    = emitter.child("startSpeed").attribute("value").as_float();
    emi.start_speed_.rand_min_ = emitter.child("startSpeed").attribute("randMin").as_float();
    emi.start_speed_.rand_max_ = emitter.child("startSpeed").attribute("randMax").as_float();

    emi.end_speed_.value_    = emitter.child("endSpeed").attribute("value").as_float();
    emi.end_speed_.rand_min_ = emitter.child("endSpeed").attribute("randMin").as_float();
    emi.end_speed_.rand_max_ = emitter.child("endSpeed").attribute("randMax").as_float();

    emi.start_size_.value_    = emitter.child("startSize").attribute("value").as_float();
    emi.start_size_.rand_min_ = emitter.child("startSize").attribute("randMin").as_float();
    emi.start_size_.rand_max_ = emitter.child("startSize").attribute("randMax").as_float();

    emi.end_size_.value_    = emitter.child("endSize").attribute("value").as_float();
    emi.end_size_.rand_min_ = emitter.child("endSize").attribute("randMin").as_float();
    emi.end_size_.rand_max_ = emitter.child("endSize").attribute("randMax").as_float();

    emi.emit_number_ = emitter.child("emitNumber").attribute("value").as_int();

    emi.emit_variance_.value_    = emitter.child("emitVariance").attribute("value").as_float();
    emi.emit_variance_.rand_min_ = emitter.child("emitVariance").attribute("randMin").as_float();
    emi.emit_variance_.rand_max_ = emitter.child("emitVariance").attribute("randMax").as_float();

    emi.max_particle_life_.value_    = emitter.child("maxParticleLife").attribute("value").as_float();
    emi.max_particle_life_.rand_min_ = emitter.child("maxParticleLife").attribute("randMin").as_float();
    emi.max_particle_life_.rand_max_ = emitter.child("maxParticleLife").attribute("randMax").as_float();

    emi.texture_rect_.x = emitter.child("textureRect").attribute("x").as_int();
    emi.texture_rect_.y = emitter.child("textureRect").attribute("y").as_int();
    emi.texture_rect_.w = emitter.child("textureRect").attribute("w").as_int();
    emi.texture_rect_.h = emitter.child("textureRect").attribute("h").as_int();

    emi.start_color_.r = emitter.child("startColor").attribute("r").as_uint();
    emi.start_color_.g = emitter.child("startColor").attribute("g").as_uint();
    emi.start_color_.g = emitter.child("startColor").attribute("b").as_uint();
    emi.start_color_.a = emitter.child("startColor").attribute("a").as_uint();

    emi.end_color_.r = emitter.child("endColor").attribute("r").as_uint();
    emi.end_color_.g = emitter.child("endColor").attribute("g").as_uint();
    emi.end_color_.g = emitter.child("endColor").attribute("b").as_uint();
    emi.end_color_.a = emitter.child("endColor").attribute("a").as_uint();

    const std::string blend{emitter.child("blendMode").attribute("mode").as_string()};
    if (type == std::string{"add"}) {
      emi.blend_mode_ = SDL_BLENDMODE_ADD;
    } else if (type == std::string{"blend"}) {
      emi.blend_mode_ = SDL_BLENDMODE_BLEND;
    } else if (type == std::string{"mod"}) {
      emi.blend_mode_ = SDL_BLENDMODE_MOD;
    } else {
      emi.blend_mode_ = SDL_BLENDMODE_NONE;
    }

    emi.life_time_ = emitter.child("lifeTime").attribute("value").as_int();

    emitter_types.push_back(emi);
  }
  logMessage("Loaded " + std::to_string(emitter_types.size()) + " particle emitter configurations: " + types_created);
}

bool ktp::XMLParser::initEmitters() {
  loadEmitterTypes();
  return true;
}

void ktp::XMLParser::loadEmitterTypes() {
  constexpr auto file = "emitters.xml";
  const auto path = getResourcesPath("particles") + file;
  pugi::xml_document doc{};
  const auto result = doc.load_file(path.c_str());
  if (result) {
    // printLoadedEmitterTypes(doc);
    constructEmitterTypesVector(doc);
  } else {
    std::stringstream error_msg{};
    error_msg << "WARNING! " << file << " parsed with errors\n"
              << "Error description: " << result.description() << '\n'
              << "Error offset: " << result.offset;
    logErrorMessage(error_msg.str(), path);
  }
}

void ktp::XMLParser::printLoadedEmitterTypes(const pugi::xml_document& doc) {
  const auto emitters = doc.child("emitterTypes");
  std::stringstream final_string{};
  for (const auto& emitter: emitters) {
    final_string << "<emitter ";
    for (const auto& attr: emitter.attributes()) {
      final_string << attr.name() << "=\"" << attr.value() << "\">\n";
    }
    for (const auto& child: emitter.children()) {
      final_string << " <" << child.name();

      for (const auto& child_attr: child.attributes()){
        final_string << ' ' << child_attr.name() << "=\"" << child_attr.value() << '\"';
      }
      final_string << "/>\n";
    }
    final_string << "</emitter>\n";
  }
  logMessage(final_string.str());
}

/* PARTICLE */

void ktp::Particle::draw(const SDL2_Renderer& renderer) const {
  if (frames_left_ > 150) {
    renderer.setDrawColor(ktp::Colors::white);
  } else if (frames_left_ > 100) {
    renderer.setDrawColor(ktp::Colors::yellow);
  } else if (frames_left_ > 50) {
    renderer.setDrawColor(ktp::Colors::red);
  } else {
    renderer.setDrawColor(ktp::Colors::violet);
  }
  //renderer.setDrawColor(ktp::Colors::yellow);
  renderer.drawPoint(state_.live_.position_);
}

void ktp::Particle::init(const SDL_FPoint& pos, const SDL_FPoint& delta, int life_time) {
  state_.live_.position_ = pos;
  state_.live_.delta_ = delta;
  frames_left_ = life_time;
}

/* return true if the previously live particle gave up the ghost in that frame */
bool ktp::Particle::update(float delta_time) {
  if (!inUse()) return false;

  --frames_left_;
  state_.live_.position_.x += state_.live_.delta_.x * delta_time;
  state_.live_.position_.y += state_.live_.delta_.y * delta_time;
  return frames_left_ == 0;
}

/* PARTICLEPOOL */

ktp::ParticlePool::ParticlePool(int pool_size): kPoolSize_(pool_size) {
  particles_ = new Particle[kPoolSize_];
  first_available_ = &particles_[0];

  for (auto i = 0; i < kPoolSize_; ++i) {
    particles_[i].setNext(&particles_[i + 1]);
  }
  particles_[kPoolSize_ - 1].setNext(nullptr);
}

void ktp::ParticlePool::generate(const SDL_FPoint& pos, const SDL_FPoint& delta, int life_time) {
  if (first_available_ == nullptr) return;

  Particle* new_particle = first_available_;
  first_available_ = new_particle->getNext();
  new_particle->init(pos, delta, life_time);
}

void ktp::ParticlePool::draw(const SDL2_Renderer& renderer) const {
  for (auto i = 0; i < kPoolSize_; ++i) {
    if (particles_[i].inUse()) {
      particles_[i].draw(renderer);
    }
  }
}

void ktp::ParticlePool::update(float delta_time) {
  for (auto i = 0; i < kPoolSize_; ++i) {
    if (particles_[i].update(delta_time)) {
      particles_[i].setNext(first_available_);
      first_available_ = &particles_[i];
    }
  }
}

/* EMITTER */

ktp::Emitter::Emitter(EmitterTypes type) {
  for (const auto& emitter_type: XMLParser::emitter_types) {
    if (emitter_type.type_ == type) {
      type_              = emitter_type.type_;
      angle_range_       = emitter_type.angle_range_;
      rotation_speed_    = emitter_type.rotation_speed_;
      start_speed_       = emitter_type.start_speed_;
      end_speed_         = emitter_type.end_speed_;
      start_size_        = emitter_type.start_size_;
      end_size_          = emitter_type.end_size_;
      emit_number_       = emitter_type.emit_number_;
      emit_variance_     = emitter_type.emit_variance_;
      max_particle_life_ = emitter_type.max_particle_life_;
      texture_rect_      = emitter_type.texture_rect_;
      start_color_       = emitter_type.start_color_;
      end_color_         = emitter_type.end_color_;
      blend_mode_        = emitter_type.blend_mode_;
      life_time_         = emitter_type.life_time_;
    }
  }
}