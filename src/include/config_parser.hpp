#pragma once

#include "../../lib/pugixml1.11/pugixml.hpp"
#include <string>
#include <vector>

namespace ktp {

struct EmitterType;

namespace ConfigParser {

  void constructEmitterTypesVector(const pugi::xml_document& doc);

  bool loadConfigFiles();

  inline bool loadAerolitesConfig() { return true; }
  bool loadEmittersConfig();
  bool loadPlayerConfig();
  inline bool loadProjectilesConfig() { return true; }

  void printLoadedEmitterTypes(const pugi::xml_document& doc);

  static const std::string kAerolitesFile {"aerolites.xml"};
  static const std::string kEmittersFile {"emitters.xml"};
  static const std::string kPlayerFile {"player.xml"};
  static const std::string kProjectilesFile {"projectiles.xml"};

  // defined in config_parser.cpp
  extern std::vector<EmitterType> emitter_types;

} // namespace ConfigParser

} // namespace ktp