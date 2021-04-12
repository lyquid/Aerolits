#ifndef AEROLITS_SRC_EMITTER_PARSER_H_
#define AEROLITS_SRC_EMITTER_PARSER_H_

#include "../include/pugixml1.11/pugixml.hpp"
#include "../include/resources_path.hpp"
#include "../sdl2_wrappers/sdl2_wrappers.hpp"

namespace ktp {

struct AngleRange { float max_{0}, min_{0}; };

template <typename T>
struct RandomRangeValue { T value_{0}; float rand_min_{0}, rand_max_{1}; };
using RRVInt = RandomRangeValue<int>;
using RRVUint = RandomRangeValue<unsigned int>;
using RRVFloat = RandomRangeValue<float>;

enum class EmitterTypes {
  Exhaust,
  Fire,
  Love,
  Smoke,
  count
};

struct EmitterType {
  // Emitter properties
  EmitterTypes  type_{};
  AngleRange    angle_range_{};
  SDL_BlendMode blend_mode_{};
  RRVUint       emission_interval_{};
  RRVUint       emission_rate_{};
  int           life_time_{};
  SDL_Rect      texture_rect_{};
  bool          vortex_{};
  float         vortex_scale_{};
  float         vortex_speed_{};
  
  // Particles properties
  RRVUint   max_particle_life_{};
  SDL_Color start_color_{};
  SDL_Color end_color_{};
  RRVFloat  start_size_{};
  RRVFloat  end_size_{};
  RRVFloat  start_speed_{};
  RRVFloat  end_speed_{};
  RRVFloat  rotation_{};
  RRVFloat  start_rotation_speed_{};
  RRVFloat  end_rotation_speed_{};
};

namespace EmitterParser {

  void constructEmitterTypesVector(const pugi::xml_document& doc);
  bool initEmitters();
  void loadEmitterTypes();
  void printLoadedEmitterTypes(const pugi::xml_document& doc);
  extern std::vector<EmitterType> emitter_types;

} // end namespace EmitterParser

} // end namespace ktp

#endif // AEROLITS_SRC_EMITTER_PARSER_H_