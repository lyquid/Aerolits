#ifndef KUGE_HEADERS_AUDIO_SYSTEM_H_
#define KUGE_HEADERS_AUDIO_SYSTEM_H_

#include "event.hpp"
#include "system.hpp"
#include "../sdl2_wrappers/sdl2_wrappers.hpp"
#include "../src/include/resources_path.hpp"
#include <array>
#include <iterator>
#include <random>
#include <vector>

namespace kuge {

class AudioSystem : virtual public System {
 public:
  AudioSystem(EventBus& bus): System(bus) {}
  void handleEvent(const Event& event);
  static bool loadResources();

 private:
  static void generateRandomSequence();
  /* Lasers */
  using laser_randomizer = std::array<unsigned int, 400>;
  using laser_randomizer_it = laser_randomizer::const_iterator;

  static std::vector<ktp::SDL2_Sound> lasers_;
  static laser_randomizer             lasers_sequence_;
  static laser_randomizer_it          lasers_it_;
};

} // end namespace kuge

#endif // KUGE_HEADERS_AUDIO_SYSTEM_H_