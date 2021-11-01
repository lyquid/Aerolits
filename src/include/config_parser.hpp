#pragma once

#include "palette.hpp"
#include "../../lib/pugixml1.11/pugixml.hpp"
#include <SDL.h>
#include <string>
#include <vector>

namespace ktp {

struct AngleRange { float max_{0}, min_{0}; };

template<typename T> struct RandomRangeValue { T value_{0}; float rand_min_{0}, rand_max_{1}; };
using RRVInt   = RandomRangeValue<int>;
using RRVUint  = RandomRangeValue<unsigned int>;
using RRVFloat = RandomRangeValue<float>;

using ColorsVector = std::vector<SDL_Color>;
using RRVFVector   = std::vector<RRVFloat>;

struct EmitterType;

namespace ConfigParser {

  inline static const std::string kAerolitesFile {"aerolites.xml"};
  inline static const std::string kEmittersFile {"emitters.xml"};
  inline static const std::string kGameFile {"game.xml"};
  inline static const std::string kPlayerFile {"player.xml"};
  inline static const std::string kProjectilesFile {"projectiles.xml"};

  /**
   * @brief Checks if a number is between a range.
   * @tparam T Any numeric value.
   * @param number The number to check if it's between a range.
   * @param lower The lowest number in the range.
   * @param upper The highest number in the range.
   * @return True if the number is within range. False otherwise.
   */
  template<class T>
  inline auto checkWithinRange(T number, T lower, T upper) {
    return number >= lower && number <= upper;
  }

  void loadConfigFiles();

  // AEROLITES
  struct AerolitesConfig {
    ColorsVector colors_ {Colors::orange};
    float density_ {3.f};
    float friction_ {1.f};
    float restitution_ {0.f};
    RRVFloat rotation_speed_ {2.f, -1.f, 1.f};
    RRVFloat size_ {3.0f, 0.6f, 1.f};
    RRVFloat speed_ {10.f, 0.1f, 1.f};
  };
  extern AerolitesConfig aerolites_config;
  void loadAerolitesConfig();

  // EMITTERS

  void constructEmitterTypesVector(const pugi::xml_document& doc);
  void loadEmittersConfig();
  void printLoadedEmitterTypes(const pugi::xml_document& doc);
  extern std::vector<EmitterType> emitter_types;

  // GAME

  struct GameConfig {
    bool output_ {true};
    SDL_Point screen_size_ {1366, 768};
  };
  extern GameConfig game_config;
  void loadGameConfig();

  // PLAYER

  /**
   * @brief A struct with some attributes that represents the player.
   */
  struct PlayerConfig {
    SDL_Color color_ {Colors::white};
    float density_ {1.5f};
    float friction_ {0.8f};
    float restitution_ {0.f};
    float size_ {1.2f};
    float angular_impulse_ {5.f};
    float linear_impulse_ {0.05f};
    float max_delta_ {0.1f};
  };
  extern PlayerConfig player_config;
  void loadPlayerConfig();

  // PROJECTILES

  struct ExplosionConfig {
    float blast_power_ {50.f};
    float density_ {60.f};
    unsigned int duration_ {500u};
    float friction_ {0.f};
    float linear_damping_ {5.f};
    float particle_radius_ {0.2f};
    unsigned int rays_ {100u};
    float restitution_ {0.99f};
  };

  struct ProjectilesConfig {
    unsigned int arm_time_ {300u};
    SDL_Color color_ {Colors::copper_green};
    float density_ {10.f};
    float friction_ {0.1f};
    float restitution_ {0.35f};
    float size_ {0.15f};
    float speed_ {0.1f};
    // explosion
    ExplosionConfig explosion_config_ {};
  };
  extern ProjectilesConfig projectiles_config;
  void loadProjectilesConfig();

} // namespace ConfigParser

} // namespace ktp