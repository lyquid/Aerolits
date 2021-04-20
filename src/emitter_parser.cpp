#include "palette.hpp"
#include "emitter_parser.hpp"
#include <sstream>
#include <string>
#include <vector>

std::vector<ktp::EmitterType> ktp::EmitterParser::emitter_types{};

void ktp::EmitterParser::constructEmitterTypesVector(const pugi::xml_document& doc) {
  const auto emitters{doc.child("emitterTypes")};
  std::string types_created{};
  for (const auto& emitter: emitters) {
    EmitterType emi{};
    /* EMITTER TYPE */
    const std::string type{emitter.attribute("type").as_string()};
    types_created += (type + ' ');
    if (type == std::string{"exhaust"}) {
      emi.type_ = EmitterTypes::Exhaust;
    } else if (type == std::string{"fire"}) {
      emi.type_ = EmitterTypes::Fire;
    } else if (type == std::string{"love"}) {
      emi.type_ = EmitterTypes::Love;
    } else if (type == std::string{"smoke"}) {
      emi.type_ = EmitterTypes::Smoke;
    } else {
      logErrorMessage("Unknown emitter type");
      return;
    }
    /* ANGLE RANGE - the "magic number" is to compensate the inverted y coordinate and put 0º at the top: 45º + 90º */
    #ifndef M_PI
      constexpr auto kPI{3.14159265358979323846264338327950288};
      emi.angle_range_.min_ = (emitter.child("angleRange").attribute("min").as_float() - 135.f) * kPI / 180.f;
      emi.angle_range_.max_ = (emitter.child("angleRange").attribute("max").as_float() - 135.f) * kPI / 180.f;
    #else
      emi.angle_range_.min_ = (emitter.child("angleRange").attribute("min").as_float() - 135.f) * M_PI / 180.f;
      emi.angle_range_.max_ = (emitter.child("angleRange").attribute("max").as_float() - 135.f) * M_PI / 180.f;
    #endif
    /* BLEND MODE */
    const std::string blend{emitter.child("blendMode").attribute("mode").as_string()};
    if (blend == std::string{"add"} || blend == std::string{"ADD"}) {
      emi.blend_mode_ = SDL_BLENDMODE_ADD;
    } else if (blend == std::string{"blend"} || blend == std::string{"BLEND"}) {
      emi.blend_mode_ = SDL_BLENDMODE_BLEND;
    } else if (blend == std::string{"mod"} || blend == std::string{"MOD"}) {
      emi.blend_mode_ = SDL_BLENDMODE_MOD;
    } else {
      emi.blend_mode_ = SDL_BLENDMODE_NONE;
    }
    /* EMISSION INTERVAL */
    if (emitter.child("emissionInterval").attribute("value").as_uint() < 0) {
      logMessage("WARNING! Emitter \"" + type + "\" has less than 0 emission interval.");
    }
    emi.emission_interval_.value_     = emitter.child("emissionInterval").attribute("value").as_uint();
    emi.emission_interval_.rand_min_  = emitter.child("emissionInterval").attribute("randMin").as_float();
    emi.emission_interval_.rand_max_  = emitter.child("emissionInterval").attribute("randMax").as_float();
    /* EMISSION RATE */
    if (emitter.child("emissionRate").attribute("value").as_uint() <= 0u) {
      logMessage("WARNING! Emitter \"" + type + "\" has 0 or less emission rate.");
    }
    emi.emission_rate_.value_    = emitter.child("emissionRate").attribute("value").as_uint();
    emi.emission_rate_.rand_min_ = emitter.child("emissionRate").attribute("randMin").as_float();
    emi.emission_rate_.rand_max_ = emitter.child("emissionRate").attribute("randMax").as_float();
    /* EMITTER'S LIFE TIME */
    if (emitter.child("lifeTime").attribute("value").as_int() == 0) {
      logMessage("WARNING! Emitter \"" + type + "\" has 0 life time.");
    }
    emi.life_time_ = emitter.child("lifeTime").attribute("value").as_int();
    /* TEXTURE RECTANGLE */
    if (emitter.child("textureRect").attribute("x").as_int() < 0 
     || emitter.child("textureRect").attribute("y").as_int() < 0 
     || emitter.child("textureRect").attribute("w").as_int() < 0 
     || emitter.child("textureRect").attribute("h").as_int() < 0 ) {
        logMessage("WARNING! Emitter \"" + type + "\" has an imposible texture rectangle.");
    }
    emi.texture_rect_.x = emitter.child("textureRect").attribute("x").as_int();
    emi.texture_rect_.y = emitter.child("textureRect").attribute("y").as_int();
    emi.texture_rect_.w = emitter.child("textureRect").attribute("w").as_int();
    emi.texture_rect_.h = emitter.child("textureRect").attribute("h").as_int();
    /* VORTEX */
    emi.vortex_ = emitter.child("vortex").attribute("active").as_bool();
    if (emi.vortex_) {
      emi.vortex_scale_ = emitter.child("vortex").child("scale").attribute("value").as_float();
      emi.vortex_speed_ = emitter.child("vortex").child("speed").attribute("value").as_float();
    }
    /* PARTICLE LIFE */
    if (emitter.child("maxParticleLife").attribute("value").as_uint() <= 0) {
      logMessage("WARNING! Emitter \"" + type + "\" has a particle starting life of 0 or less.");
    }
    emi.max_particle_life_.value_    = emitter.child("maxParticleLife").attribute("value").as_uint();
    emi.max_particle_life_.rand_min_ = emitter.child("maxParticleLife").attribute("randMin").as_float();
    emi.max_particle_life_.rand_max_ = emitter.child("maxParticleLife").attribute("randMax").as_float();
    /* COLORS */
    auto it = emitter.child("colors").begin();
    while (it != emitter.child("colors").end()) {
      const SDL_Color requested_color {
        static_cast<Uint8>(it->attribute("r").as_uint()),
        static_cast<Uint8>(it->attribute("g").as_uint()),
        static_cast<Uint8>(it->attribute("b").as_uint()),
        static_cast<Uint8>(it->attribute("a").as_uint())
      };
      SDL_Color final_color {Colors::getNearestColor(requested_color)};
      final_color.a = requested_color.a;
      emi.colors_.push_back(final_color);
      ++it;
    }
    if (emi.colors_.size() > 3) {
      logMessage("WARNING! Emitter \"" + type + "\" has more than 3 colors, but only the first 3 will be used for interpolation.");
    }
    /* START SIZE */
    if (emitter.child("startSize").attribute("value").as_float() < 0) {
      logMessage("WARNING! Emitter \"" + type + "\" has a negative start size.");
    }
    emi.start_size_.value_    = emitter.child("startSize").attribute("value").as_float();
    emi.start_size_.rand_min_ = emitter.child("startSize").attribute("randMin").as_float();
    emi.start_size_.rand_max_ = emitter.child("startSize").attribute("randMax").as_float();
    /* END SIZE */
    if (emitter.child("endSize").attribute("value").as_float() < 0) {
      logMessage("WARNING! Emitter \"" + type + "\" has a negative end size.");
    }
    emi.end_size_.value_    = emitter.child("endSize").attribute("value").as_float();
    emi.end_size_.rand_min_ = emitter.child("endSize").attribute("randMin").as_float();
    emi.end_size_.rand_max_ = emitter.child("endSize").attribute("randMax").as_float();
    /* SPEEDS */
    it = emitter.child("speeds").begin();
    while (it != emitter.child("speeds").end()) {
      const RRVFloat speed {
        it->attribute("value").as_float(),
        it->attribute("randMin").as_float(),
        it->attribute("randMax").as_float()
      };
      emi.speeds_.push_back(speed);
      ++it;
    }
    if (emi.speeds_.size() > 3) {
      logMessage("WARNING! Emitter \"" + type + "\" has more than 3 speeds, but only the first 3 will be used for interpolation.");
    }
    /* ROTATION */
    emi.rotation_.value_    = emitter.child("rotation").attribute("value").as_float();
    emi.rotation_.rand_min_ = emitter.child("rotation").attribute("randMin").as_float();
    emi.rotation_.rand_max_ = emitter.child("rotation").attribute("randMax").as_float();
    /* START ROTATION SPEED */
    emi.start_rotation_speed_.value_    = emitter.child("startRotationSpeed").attribute("value").as_float();
    emi.start_rotation_speed_.rand_min_ = emitter.child("startRotationSpeed").attribute("randMin").as_float();
    emi.start_rotation_speed_.rand_max_ = emitter.child("startRotationSpeed").attribute("randMax").as_float();
    /* END ROTATION SPEED */ 
    emi.end_rotation_speed_.value_    = emitter.child("endRotationSpeed").attribute("value").as_float();
    emi.end_rotation_speed_.rand_min_ = emitter.child("endRotationSpeed").attribute("randMin").as_float();
    emi.end_rotation_speed_.rand_max_ = emitter.child("endRotationSpeed").attribute("randMax").as_float();

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
      final_string << "\t<" << child.name();

      for (const auto& child_attr: child.attributes()){
        final_string << ' ' << child_attr.name() << "=\"" << child_attr.value() << '\"';
      }
      if (SDL_strcmp(child.name(), "vortex") == 0) {
        final_string << ">\n";
        for (const auto& vortex_childs: child.children()) {
          final_string << "\t\t<" << vortex_childs.name();

          for (const auto& vortex_childs_attrs: vortex_childs.attributes()) {
            final_string << ' ' << vortex_childs_attrs.name() << "=\"" << vortex_childs_attrs.value() << "\"/>\n";
          }
        }
        final_string << "\t</vortex>\n";
      } else if (SDL_strcmp(child.name(), "colors") == 0) {
        final_string << ">\n";
        for (const auto& colors_childs: child.children()) {
          final_string << "\t\t<" << colors_childs.name();

          for (const auto& colors_childs_attrs: colors_childs.attributes()) {
            final_string << ' ' << colors_childs_attrs.name() << "=\"" << colors_childs_attrs.value() << "\"";
          }
          final_string << "/>\n";
        }
        final_string << "\t</colors>\n";
      } else if (SDL_strcmp(child.name(), "speeds") == 0) {
        final_string << ">\n";
        for (const auto& speeds_childs: child.children()) {
          final_string << "\t\t<" << speeds_childs.name();

          for (const auto& speeds_childs_attrs: speeds_childs.attributes()) {
            final_string << ' ' << speeds_childs_attrs.name() << "=\"" << speeds_childs_attrs.value() << "\"";
          }
          final_string << "/>\n";
        }
        final_string << "\t</speeds>\n";
      } else {
        final_string << "/>\n";
      }
    }
    final_string << "</emitter>\n";
  }
  logMessage(final_string.str());
}