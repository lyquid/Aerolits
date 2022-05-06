#ifndef KUGE_HEADERS_GUI_SYSTEM_HPP_
#define KUGE_HEADERS_GUI_SYSTEM_HPP_

#include "system.hpp"
#include "../include/opengl.hpp"
#include "../include/palette.hpp"

namespace kuge {

struct GUIStringConfig {
  // The desired name for the texture.
  std::string name_ {};
  // The text to display.
  std::string text_ {};
  // The font to use.
  std::string font_ {};
  // The color for the font.
  ktp::Color color_ {};
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

  // inline static const std::string kDemoModeText_ {"DEMOSTRACIÓ"};
  // inline static const std::string kPausedText_ {"PAUSAT"};
  // inline static const std::string kScoreText_ {"PUNTUACIÓ "};
  inline static const std::string kDemoModeText_ {"DEMO"};
  inline static const std::string kPausedText_ {"PAUSED"};
  inline static const std::string kScoreText_ {"SCORE "};
  inline static const std::string kTitleText_ {"Aeròlits"};

 private:

  GUIString demo_text_ {};
  GUIString paused_text_ {};
  GUIString score_text_ {};
  GUIString title_text_ {};

  Uint32 score_ {0};
};

} // namespace kuge

#endif // KUGE_HEADERS_GUI_SYSTEM_HPP_
