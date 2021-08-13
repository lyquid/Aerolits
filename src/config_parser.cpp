#include "include/config_parser.hpp"
#include "include/emitter.hpp"
#include "include/paths.hpp"
#include "../sdl2_wrappers/sdl2_log.hpp"
#include <algorithm> // std::transform
#include <sstream> // std::ostringstream

void ktp::ConfigParser::loadConfigFiles() {
  loadAerolitesConfig();
  loadEmittersConfig();
  loadGameConfig();
  loadPlayerConfig();
  loadProjectilesConfig();
}

// AEROLITES

ktp::ConfigParser::AerolitesConfig ktp::ConfigParser::aerolites_config {};

void ktp::ConfigParser::loadAerolitesConfig() {
  const std::string path {getConfigPath() + kAerolitesFile};
  pugi::xml_document doc {};
  const auto result {doc.load_file(path.c_str())};
  if (result) {
    const auto aerolites {doc.child("aerolites")};
    // Colors
    if (!aerolites.child("colors").empty()) {
      aerolites_config.colors_.clear();
      auto color = aerolites.child("colors").begin();
      while (color != aerolites.child("colors").end()) {
        const SDL_Color requested_color {
          (Uint8)color->attribute("r").as_uint(),
          (Uint8)color->attribute("g").as_uint(),
          (Uint8)color->attribute("b").as_uint(),
          (Uint8)255
        };
        SDL_Color final_color {Colors::getNearestColor(requested_color)};
        final_color.a = requested_color.a;
        aerolites_config.colors_.push_back(final_color);
        ++color;
      }
      aerolites_config.colors_.shrink_to_fit();
    }
    // Density
    if (aerolites.child("Box2D").child("density")) {
      const auto density {aerolites.child("Box2D").child("density").attribute("value").as_float()};
      if (density >= 0.f) {
        aerolites_config.density_ = density;
      } else {
        logMessage("Warning! Aerolites density less than 0. Using default density.");
      }
    } else {
      logMessage("Warning! Aerolites density not set. Using default density.");
    }
    // Friction
    if (aerolites.child("Box2D").child("friction")) {
      const auto friction {aerolites.child("Box2D").child("friction").attribute("value").as_float()};
      if (checkWithinRange(friction, 0.f, 1.f)) {
        aerolites_config.friction_ = friction;
      } else {
        logMessage("Warning! Aerolites friction not in range [0, 1]. Using default friction.");
      }
    } else {
      logMessage("Warning! Aerolites friction not set. Using default friction.");
    }
    // Restitution
    if (aerolites.child("Box2D").child("restitution")) {
      const auto restitution {aerolites.child("Box2D").child("restitution").attribute("value").as_float()};
      if (checkWithinRange(restitution, 0.f, 1.f)) {
        aerolites_config.restitution_ = restitution;
      } else {
        logMessage("Warning! Aerolites restitution not in range [0, 1]. Using default restitution.");
      }
    } else {
      logMessage("Warning! Aerolites restitution not set. Using default restitution.");
    }
    // Rotation speed
    if (aerolites.child("rotationSpeed")) {
      const RRVFloat rotation_speed {
        aerolites.child("rotationSpeed").attribute("value").as_float(),
        aerolites.child("rotationSpeed").attribute("randMin").as_float(),
        aerolites.child("rotationSpeed").attribute("randMax").as_float()
      };
      if (rotation_speed.value_ >= 0 && rotation_speed.rand_min_ <= rotation_speed.rand_max_ && checkWithinRange(rotation_speed.rand_min_, -1.f, 1.f) && checkWithinRange(rotation_speed.rand_max_, -1.f, 1.f)) {
        aerolites_config.rotation_speed_ = rotation_speed;
      } else {
        logMessage("Warning! Aerolites rotation speed ill-formed. Using default rotation speed.");
      }
    } else {
      logMessage("Warning! Aerolites rotation speed not set. Using default rotation speed.");
    }
    // Size
    if (aerolites.child("size")) {
      const RRVFloat size {
        aerolites.child("size").attribute("value").as_float(),
        aerolites.child("size").attribute("randMin").as_float(),
        aerolites.child("size").attribute("randMax").as_float()
      };
      if (size.value_ > 0 && size.rand_min_ <= size.rand_max_ && checkWithinRange(size.rand_min_, 0.f, 1.f) && checkWithinRange(size.rand_max_, 0.f, 1.f)) {
        aerolites_config.size_ = size;
      } else {
        logMessage("Warning! Aerolites size ill-formed. Using default size.");
      }
    } else {
      logMessage("Warning! Aerolites size not set. Using default rotation size.");
    }
    // Speed
    if (aerolites.child("speed")) {
      const RRVFloat speed {
        aerolites.child("speed").attribute("value").as_float(),
        aerolites.child("speed").attribute("randMin").as_float(),
        aerolites.child("speed").attribute("randMax").as_float()
      };
      if (speed.value_ > 0 && speed.rand_min_ <= speed.rand_max_ && checkWithinRange(speed.rand_min_, 0.f, 1.f) && checkWithinRange(speed.rand_max_, 0.f, 1.f)) {
        aerolites_config.speed_ = speed;
      } else {
        logMessage("Warning! Aerolites speed ill-formed. Using default speed.");
      }
    } else {
      logMessage("Warning! Aerolites speed not set. Using default rotation speed.");
    }

  } else {
    const std::string error_msg {
            "WARNING! " + kAerolitesFile + " parsed with errors\n"
            + "Error description: " + result.description() + '\n'
            + "Error offset: " + std::to_string(result.offset)};
    logError(error_msg, path);
  }
}

// EMITTERS

std::vector<ktp::EmitterType> ktp::ConfigParser::emitter_types {};

void ktp::ConfigParser::constructEmitterTypesVector(const pugi::xml_document& doc) {
  const auto emitters {doc.child("emitterTypes")};
  std::string types_created {};
  for (const auto& emitter: emitters) {
    EmitterType emi {};
    /* EMITTER TYPE */
    std::string type {emitter.attribute("type").as_string()};
    std::transform(type.begin(), type.end(), type.begin(), ::tolower);
    types_created += (type + ' ');
    emi.type_ = type;
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
    std::string blend{emitter.child("blendMode").attribute("mode").as_string()};
    std::transform(blend.begin(), blend.end(), blend.begin(), ::tolower);
    if (blend == std::string{"add"}) {
      emi.blend_mode_ = SDL_BLENDMODE_ADD;
    } else if (blend == std::string{"blend"}) {
      emi.blend_mode_ = SDL_BLENDMODE_BLEND;
    } else if (blend == std::string{"mod"}) {
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
      if (emitter.child("vortex").child("scale").attribute("value").as_float() == 0.f) {
        logMessage("WARNING! Emitter \"" + type + "\" has a vortex with 0 scale.");
      }
      emi.vortex_speed_ = emitter.child("vortex").child("speed").attribute("value").as_float();
      if (emitter.child("vortex").child("speed").attribute("value").as_float() == 0.f) {
        logMessage("WARNING! Emitter \"" + type + "\" has a vortex with 0 speed.");
      }
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
    /* SIZES */
    it = emitter.child("sizes").begin();
    while (it != emitter.child("sizes").end()) {
      const RRVFloat size {
        it->attribute("value").as_float(),
        it->attribute("randMin").as_float(),
        it->attribute("randMax").as_float()
      };
      if (size.value_ < 0) {
        logMessage("WARNING! Emitter \"" + type + "\" has a negative size.");
      }
      emi.sizes_.push_back(size);
      ++it;
    }
    if (emi.sizes_.size() > 3) {
      logMessage("WARNING! Emitter \"" + type + "\" has more than 3 sizes, but only the first 3 will be used for interpolation.");
    }
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

void ktp::ConfigParser::loadEmittersConfig() {
  const std::string path {getConfigPath() + kEmittersFile};
  pugi::xml_document doc {};
  const auto result {doc.load_file(path.c_str())};
  if (result) {
    // printLoadedEmitterTypes(doc);
    constructEmitterTypesVector(doc);
  } else {
    const std::string error_msg {
            "WARNING! " + kEmittersFile + " parsed with errors\n"
            + "Error description: " + result.description() + '\n'
            + "Error offset: " + std::to_string(result.offset)};
    logError(error_msg, path);
  }
}

void ktp::ConfigParser::printLoadedEmitterTypes(const pugi::xml_document& doc) {
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

// GAME

ktp::ConfigParser::GameConfig ktp::ConfigParser::game_config {};

void ktp::ConfigParser::loadGameConfig() {
  const std::string path {getConfigPath() + kGameFile};
  pugi::xml_document doc {};
  const auto result {doc.load_file(path.c_str())};
  if (result) {
    const auto game {doc.child("game")};
    // Screen size
    if (game.child("screenSize")) {
      const SDL_Point size {game.child("screenSize").attribute("x").as_int(), game.child("screenSize").attribute("y").as_int()};
      if (size.x < game_config.screen_size_.x || size.y < game_config.screen_size_.y) {
        logMessage("Warning! screen size too low. Using default size.");
      } else {
        game_config.screen_size_ = size;
      }
      game_config.screen_size_.x = game.child("screenSize").attribute("x").as_int();
      game_config.screen_size_.y = game.child("screenSize").attribute("y").as_int();
    } else {
      logMessage("Warning! screen size not set. Using default size.");
    }
  } else {
    const std::string error_msg {
            "WARNING! " + kGameFile + " parsed with errors\n"
            + "Error description: " + result.description() + '\n'
            + "Error offset: " + std::to_string(result.offset)};
    logError(error_msg, path);
  }
}

// PLAYER

ktp::ConfigParser::PlayerConfig ktp::ConfigParser::player_config {};

void ktp::ConfigParser::loadPlayerConfig() {
  const std::string path {getConfigPath() + kPlayerFile};
  pugi::xml_document doc {};
  const auto result {doc.load_file(path.c_str())};
  if (result) {
    const auto player {doc.child("player")};
    // Color
    if (player.child("color")) {
      const SDL_Color requested_color {
        (Uint8)player.child("color").attribute("r").as_uint(),
        (Uint8)player.child("color").attribute("g").as_uint(),
        (Uint8)player.child("color").attribute("b").as_uint(),
        (Uint8)255
      };
      player_config.color_ = Colors::getNearestColor(requested_color);
    } else {
      logMessage("Warning! Player color not set. Using default color.");
    }
    // Density
    if (player.child("Box2D").child("density")) {
      const auto density {player.child("Box2D").child("density").attribute("value").as_float()};
      if (density >= 0.f) {
        player_config.density_ = density;
      } else {
        logMessage("Warning! Player's density less than 0. Using default density.");
      }
    } else {
      logMessage("Warning! Player density not set. Using default density.");
    }
    // Friction
    if (player.child("Box2D").child("friction")) {
      const auto friction {player.child("Box2D").child("friction").attribute("value").as_float()};
      if (checkWithinRange(friction, 0.f, 1.f)) {
        player_config.friction_ = friction;
      } else {
        logMessage("Warning! Player's friction not in range [0, 1]. Using default friction.");
      }
    } else {
      logMessage("Warning! Player friction not set. Using default friction.");
    }
    // Restitution
    if (player.child("Box2D").child("restitution")) {
      const auto restitution {player.child("Box2D").child("restitution").attribute("value").as_float()};
      if (checkWithinRange(restitution, 0.f, 1.f)) {
        player_config.restitution_ = restitution;
      } else {
        logMessage("Warning! Player's restitution not in range [0, 1]. Using default restitution.");
      }
    } else {
      logMessage("Warning! Player restitution not set. Using default restitution.");
    }
    // Size
    if (player.child("size")) {
      const auto size {player.child("size").attribute("value").as_float()};
      if (size > 0.f) {
        player_config.size_ = size;
      } else {
        logMessage("Warning! Player's size 0 or less. Using default size.");
      }
    } else {
      logMessage("Warning! Player size not set. Using default size.");
    }
  } else {
    const std::string error_msg {
            "WARNING! " + kPlayerFile + " parsed with errors\n"
            + "Error description: " + result.description() + '\n'
            + "Error offset: " + std::to_string(result.offset)};
    logError(error_msg, path);
  }
}

// PROJECTILES

ktp::ConfigParser::ProjectilesConfig ktp::ConfigParser::projectiles_config {};

void ktp::ConfigParser::loadProjectilesConfig() {
  const std::string path {getConfigPath() + kProjectilesFile};
  pugi::xml_document doc {};
  const auto result {doc.load_file(path.c_str())};
  if (result) {
    const auto projectiles {doc.child("projectiles")};
    // Color
    if (projectiles.child("color")) {
      const SDL_Color requested_color {
        (Uint8)projectiles.child("color").attribute("r").as_uint(),
        (Uint8)projectiles.child("color").attribute("g").as_uint(),
        (Uint8)projectiles.child("color").attribute("b").as_uint(),
        (Uint8)255
      };
      projectiles_config.color_ = Colors::getNearestColor(requested_color);
    } else {
      logMessage("Warning! Projectiles color not set. Using default color.");
    }
    // Density
    if (projectiles.child("Box2D").child("density")) {
      const auto density {projectiles.child("Box2D").child("density").attribute("value").as_float()};
      if (density > 0) {
        projectiles_config.density_ = density;
      } else {
        logMessage("Warning! Projectiles density less than 0. Using default density.");
      }
    } else {
      logMessage("Warning! Projectiles density not set. Using default density.");
    }
    // Friction
    if (projectiles.child("Box2D").child("friction")) {
      const auto friction {projectiles.child("Box2D").child("friction").attribute("value").as_float()};
      if (checkWithinRange(friction, 0.f, 1.f)) {
        projectiles_config.friction_ = friction;
      } else {
        logMessage("Warning! Projectiles friction not in range [0, 1]. Using default friction.");
      }
    } else {
      logMessage("Warning! Projectiles friction not set. Using default friction.");
    }
    // Restitution
    if (projectiles.child("Box2D").child("restitution")) {
      const auto restitution {projectiles.child("Box2D").child("restitution").attribute("value").as_float()};
      if (checkWithinRange(restitution, 0.f, 1.f)) {
        projectiles_config.restitution_ = restitution;
      } else {
        logMessage("Warning! Projectiles restitution not in range [0, 1]. Using default restitution.");
      }
    } else {
      logMessage("Warning! Projectiles restitution not set. Using default restitution.");
    }
    // Size
    if (projectiles.child("size")) {
      const auto size {projectiles.child("size").attribute("value").as_float()};
      if (size > 0) {
        projectiles_config.size_ = size;
      } else {
        logMessage("Warning! Projectiles size 0 or less. Using default size.");
      }
    } else {
      logMessage("Warning! Projectiles size not set. Using default size.");
    }
    // Speed
    if (projectiles.child("speed")) {
      const auto speed {projectiles.child("speed").attribute("value").as_float()};
      if (speed >= 0) {
        projectiles_config.speed_ = speed;
      } else {
        logMessage("Warning! Projectiles speed less than 0. Using default speed.");
      }
    } else {
      logMessage("Warning! Projectiles speed not set. Using default speed.");
    }
    // Blast power
    if (projectiles.child("explosion").child("blastPower")) {
      const auto blast_power {projectiles.child("explosion").child("blastPower").attribute("value").as_float()};
      if (blast_power >= 0) {
        projectiles_config.explosion_config_.blast_power_ = blast_power;
      } else {
        logMessage("Warning! Projectiles explosion blast power less than 0. Using default blast power.");
      }
    } else {
      logMessage("Warning! Projectiles explosion blast power not set. Using default blast power.");
    }
    // Explosion density
    if (projectiles.child("explosion").child("density")) {
      const auto density {projectiles.child("explosion").child("density").attribute("value").as_float()};
      if (density >= 0) {
        projectiles_config.explosion_config_.density_ = density;
      } else {
        logMessage("Warning! Projectiles explosion density less than 0. Using default density.");
      }
    } else {
      logMessage("Warning! Projectiles explosion density not set. Using default density.");
    }
    // Explosion duration
    if (projectiles.child("explosion").child("duration")) {
      const auto explosion_duration {projectiles.child("explosion").child("duration").attribute("value").as_uint()};
      if (explosion_duration >= 0) {
        projectiles_config.explosion_config_.duration_ = explosion_duration;
      } else {
        logMessage("Warning! Projectiles explosion duration less than 0. Using default duration.");
      }
    } else {
      logMessage("Warning! Projectiles explosion duration not set. Using default duration.");
    }
    // Explosion friction
    if (projectiles.child("explosion").child("friction")) {
      const auto friction {projectiles.child("explosion").child("friction").attribute("value").as_float()};
      if (checkWithinRange(friction, 0.f, 1.f)) {
        projectiles_config.explosion_config_.friction_ = friction;
      } else {
        logMessage("Warning! Projectiles explosion friction not in range [0, 1]. Using default friction.");
      }
    } else {
      logMessage("Warning! Projectiles explosion friction not set. Using default friction.");
    }
    // Explosion linear damping
    if (projectiles.child("explosion").child("linearDamping")) {
      const auto damping {projectiles.child("explosion").child("linearDamping").attribute("value").as_float()};
      if (damping >= 0) {
        projectiles_config.explosion_config_.linear_damping_ = damping;
      } else {
        logMessage("Warning! Projectiles explosion linear damping less than 0. Using default linear damping.");
      }
    } else {
      logMessage("Warning! Projectiles explosion linear damping not set. Using default linear damping.");
    }
    // Explosion particle radius
    if (projectiles.child("explosion").child("particleRadius")) {
      const auto radius {projectiles.child("explosion").child("particleRadius").attribute("value").as_float()};
      if (radius > 0) {
        projectiles_config.explosion_config_.particle_radius_ = radius;
      } else {
        logMessage("Warning! Projectiles explosion particle radius 0 or less. Using default particle radius.");
      }
    } else {
      logMessage("Warning! Projectiles explosion particle radius not set. Using default particle radius.");
    }
    // Explosion rays
    if (projectiles.child("explosion").child("rays")) {
      const auto rays {projectiles.child("explosion").child("rays").attribute("value").as_uint()};
      if (rays >= 0) {
        projectiles_config.explosion_config_.rays_ = rays;
      } else {
        logMessage("Warning! Projectiles explosion rays less than 0. Using default rays.");
      }
    } else {
      logMessage("Warning! Projectiles explosion rays not set. Using default rays.");
    }
    // Explosion restitution
    if (projectiles.child("explosion").child("restitution")) {
      const auto restitution {projectiles.child("explosion").child("restitution").attribute("value").as_float()};
      if (checkWithinRange(restitution, 0.f, 1.f)) {
        projectiles_config.explosion_config_.restitution_ = restitution;
      } else {
        logMessage("Warning! Projectiles explosion restitution not in range [0, 1]. Using default restitution.");
      }
    } else {
      logMessage("Warning! Projectiles explosion restitution not set. Using default restitution.");
    }
  } else {
    const std::string error_msg {
            "WARNING! " + kProjectilesFile + " parsed with errors\n"
            + "Error description: " + result.description() + '\n'
            + "Error offset: " + std::to_string(result.offset)};
    logError(error_msg, path);
  }
}
