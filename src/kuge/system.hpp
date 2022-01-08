#ifndef KUGE_HEADERS_SYSTEM_HPP_
#define KUGE_HEADERS_SYSTEM_HPP_

#include "../sdl2_wrappers/sdl2_log.hpp"
#include "../sdl2_wrappers/sdl2_sound.hpp"
#include <array>
#include <iterator>
#include <memory>
#include <string>
#include <utility> // std::move
#include <vector>

namespace kuge {

class EventBus;
class KugeEvent;

class System {
 public:
  virtual ~System() {}
  virtual void handleEvent(const KugeEvent*) = 0;
  /* inline void postEvent(KugeEvent& event) const {
    event_bus_.postEvent(event);
  } */
 protected:
  static const EventBus* event_bus_;
};

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

class InputSystem: public System {
 public:
  virtual void handleEvent(const KugeEvent* event) override {}
};

class OutputSystem: public System {
 public:
  OutputSystem(bool log): log_(log) {}
  virtual inline void handleEvent(const KugeEvent* event) override { if (log_) event->print(); }
  inline void setLog(bool status) { log_ = status; }
 private:
  bool log_;
};

} // end namespace kuge

#endif // KUGE_HEADERS_SYSTEM_HPP_
