#include "emitter_parser.hpp"

std::vector<ktp::EmitterType> ktp::EmitterParser::emitter_types{};

void ktp::EmitterParser::constructEmitterTypesVector(const pugi::xml_document& doc) {
  const auto emitters{doc.child("emitterTypes")};
  std::string types_created{};
  for (const auto& emitter: emitters) {
    EmitterType emi{};
    
    const std::string type{emitter.attribute("type").as_string()};
    types_created += (type + ' ');
    if (type == std::string{"exhaust"}) {
      emi.type_ = EmitterTypes::Exhaust;
    } else if (type == std::string{"fire"}) {
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

    emi.max_particle_life_.value_    = emitter.child("maxParticleLife").attribute("value").as_int();
    emi.max_particle_life_.rand_min_ = emitter.child("maxParticleLife").attribute("randMin").as_float();
    emi.max_particle_life_.rand_max_ = emitter.child("maxParticleLife").attribute("randMax").as_float();

    emi.texture_rect_.x = emitter.child("textureRect").attribute("x").as_int();
    emi.texture_rect_.y = emitter.child("textureRect").attribute("y").as_int();
    emi.texture_rect_.w = emitter.child("textureRect").attribute("w").as_int();
    emi.texture_rect_.h = emitter.child("textureRect").attribute("h").as_int();

    emi.start_color_.r = emitter.child("startColor").attribute("r").as_uint();
    emi.start_color_.g = emitter.child("startColor").attribute("g").as_uint();
    emi.start_color_.b = emitter.child("startColor").attribute("b").as_uint();
    emi.start_color_.a = emitter.child("startColor").attribute("a").as_uint();

    emi.end_color_.r = emitter.child("endColor").attribute("r").as_uint();
    emi.end_color_.g = emitter.child("endColor").attribute("g").as_uint();
    emi.end_color_.b = emitter.child("endColor").attribute("b").as_uint();
    emi.end_color_.a = emitter.child("endColor").attribute("a").as_uint();

    const std::string blend{emitter.child("blendMode").attribute("mode").as_string()};
    if (blend == std::string{"add"}) {
      emi.blend_mode_ = SDL_BLENDMODE_ADD;
    } else if (blend == std::string{"blend"}) {
      emi.blend_mode_ = SDL_BLENDMODE_BLEND;
    } else if (blend == std::string{"mod"}) {
      emi.blend_mode_ = SDL_BLENDMODE_MOD;
    } else {
      emi.blend_mode_ = SDL_BLENDMODE_NONE;
    }

    emi.life_time_ = emitter.child("lifeTime").attribute("value").as_int();

    emitter_types.push_back(emi);
  }
  logMessage("Loaded " + std::to_string(emitter_types.size()) + " particle emitter configurations: " + types_created);
}

bool ktp::EmitterParser::initEmitters() {
  loadEmitterTypes();
  return true;
}

void ktp::EmitterParser::loadEmitterTypes() {
  const std::string file{"emitters.xml"};
  const std::string path{getResourcesPath("particles") + file};
  pugi::xml_document doc{};
  const auto result{doc.load_file(path.c_str())};
  if (result) {
    // printLoadedEmitterTypes(doc);
    constructEmitterTypesVector(doc);
  } else {
    const std::string error_msg{
            "WARNING! " + file + " parsed with errors\n"
            + "Error description: " + result.description() + '\n'
            + "Error offset: " + std::to_string(result.offset)};
    logErrorMessage(error_msg, path);
  }
}

void ktp::EmitterParser::printLoadedEmitterTypes(const pugi::xml_document& doc) {
  const auto emitters{doc.child("emitterTypes")};
  std::ostringstream final_string{};
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