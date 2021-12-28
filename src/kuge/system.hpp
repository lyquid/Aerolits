#ifndef KUGE_HEADERS_SYSTEM_HPP_
#define KUGE_HEADERS_SYSTEM_HPP_

#include "../sdl2_wrappers/sdl2_log.hpp"
#include "../sdl2_wrappers/sdl2_opengl.hpp"
#include "../sdl2_wrappers/sdl2_sound.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
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

struct GUIStringConfig {
  // The desired name for the texture.
  std::string name_ {};
  // The text to display.
  std::string text_ {};
  // The font to use.
  std::string font_ {};
  // The color for the font.
  SDL_Color color_ {};
  // The shader to use.
  std::string shader_ {};
  ktp::GLfloatVector vertices_;
  ktp::GLuintVector indices_;
  ktp::GLfloatVector texture_coords_;
};

class GUIStringImpl {
 public:
  GUIStringImpl(const GUIStringConfig& config);
  void draw() const;
  void updateTexture(const std::string& text);
 private:
  GUIStringConfig config_ {};
  ktp::VAO vao_ {};
  ktp::VBO vbo_ {};
  ktp::VBO uv_ {};
  ktp::EBO ebo_ {};
  ktp::ShaderProgram shader_ {};
  ktp::Texture2D texture_ {};
};

using GUIString = std::unique_ptr<GUIStringImpl>;

class GUISystem: public System {

 public:

  GUISystem() = default;
  GUISystem(const GUISystem&) = delete;
  GUISystem(GUISystem&& other) { *this = std::move(other); }
  GUISystem& operator=(const GUISystem&) = delete;
  GUISystem& operator=(GUISystem&& other) noexcept;

  virtual void handleEvent(const KugeEvent*) override;
  void init();
  void resetScore();
  inline auto score() const {return score_; }
  void updateScore(Uint32 points);

  inline auto& demoText() const { return demo_text_; }
  inline auto& pausedText() const { return paused_text_; }
  inline auto& scoreText() const { return score_text_; }
  inline auto& titleText() const { return title_text_; }

  inline static const std::string kDemoModeText_ {"DEMOSTRACIÓ"};
  inline static const std::string kPausedText_ {"PAUSAT"};
  inline static const std::string kScoreText_ {"PUNTUACIÓ "};
  inline static const std::string kTitleText_ {"Aeròlits"};

 private:

  GUIString demo_text_ {};
  GUIString paused_text_ {};
  GUIString score_text_ {};
  GUIString title_text_ {};

  Uint32 score_ {0};
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
