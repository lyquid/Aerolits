#ifndef KUGE_HEADERS_AUDIO_SYSTEM_HPP_
#define KUGE_HEADERS_AUDIO_SYSTEM_HPP_

#include "system.hpp"
#include "../sdl2_wrappers/sdl2_sound.hpp"
#include <array>
#include <iterator>
#include <vector>

namespace kuge {

class AudioSystem: public System {

  using laser_randomizer    = std::array<unsigned int, 400>;
  using laser_randomizer_it = laser_randomizer::const_iterator;

 public:

  virtual void handleEvent(const KugeEvent* event) override;
  static bool loadResources();

 private:

  static void generateRandomSequence();
  // Lasers
  static std::vector<ktp::SDL2_Sound> lasers_;
  static laser_randomizer             lasers_sequence_;
  static laser_randomizer_it          lasers_it_;
};

} // namespace kuge

#endif // KUGE_HEADERS_AUDIO_SYSTEM_HPP_
