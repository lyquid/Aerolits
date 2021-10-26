#ifndef KUGE_HEADERS_SYSTEM_HPP_
#define KUGE_HEADERS_SYSTEM_HPP_

#include "../sdl2_wrappers/sdl2_log.hpp"
#include "../sdl2_wrappers/sdl2_renderer.hpp"
#include "../sdl2_wrappers/sdl2_sound.hpp"
#include "../sdl2_wrappers/sdl2_texture.hpp"
#include <array>
#include <iterator>
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

class GUIString {
 public:
  inline void render(const ktp::SDL2_Renderer& ren) const { texture_.render(ren, rectangle_); }
  ktp::SDL2_Texture texture_ {};
  SDL_Rect rectangle_ {};
};

class GUISystem: public System {

 public:

  GUISystem(SDL_Point screen_size): screen_size_(screen_size) {}
  GUISystem(const GUISystem&) = delete;
  GUISystem(GUISystem&& other) { *this = std::move(other); }

  GUISystem& operator=(const GUISystem&) = delete;
  GUISystem& operator=(GUISystem&& other) noexcept;

  virtual void handleEvent(const KugeEvent*) override {}
  bool init(const ktp::SDL2_Renderer& ren);
  inline auto& demo() const { return demo_text_; }
  inline auto& paused() const { return paused_text_; }
  inline auto& title() const { return title_text_; }

  inline static const std::string kDemoModeText_ {"DEMO MODE"};
  inline static const std::string kPausedText_ {"PAUSED"};
  inline static const std::string kTitleText_ {"Aerolits"};

 private:

  ktp::SDL2_Font font_ {};
  SDL_Point screen_size_ {};

  GUIString demo_text_ {};
  GUIString paused_text_ {};
  GUIString title_text_ {};
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