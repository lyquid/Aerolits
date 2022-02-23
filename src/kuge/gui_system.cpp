#include "event.hpp"
#include "gui_system.hpp"
#include "../include/resources.hpp"

kuge::GUIStringImpl::GUIStringImpl(const GUIStringConfig& config) {
  config_ = config;
  texture_ = ktp::Resources::loadTextureFromTextBlended(config_.name_, config_.text_, config_.font_, ktp::Palette::colorToSDL2Color(config_.color_));
  shader_ = ktp::Resources::getShader(config_.shader_);
  vao_.bind();
  vbo_.setup(config_.vertices_);
  uv_.setup(config_.texture_coords_);
  vao_.linkAttrib(vbo_, 0, 3, GL_FLOAT, 0, nullptr);
  vao_.linkAttrib(uv_, 1, 2, GL_FLOAT, 0, nullptr);
  ebo_.setup(config_.indices_);
}

void kuge::GUIStringImpl::draw() const {
  shader_.use();
  texture_.bind();
  vao_.bind();
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void kuge::GUIStringImpl::updateTexture(const std::string& text) {
  config_.text_ = text;
  texture_ = ktp::Resources::loadTextureFromTextBlended(config_.name_, config_.text_, config_.font_, ktp::Palette::colorToSDL2Color(config_.color_));
}

kuge::GUISystem& kuge::GUISystem::operator=(GUISystem&& other) noexcept {
  if (this != &other) {
    // inherited members
    event_bus_ = std::exchange(other.event_bus_, nullptr);
    // own members
    demo_text_   = std::move(other.demo_text_);
    paused_text_ = std::move(other.paused_text_);
    score_text_  = std::move(other.score_text_);
    title_text_  = std::move(other.title_text_);
    score_       = other.score_;
  }
  return *this;
}

void kuge::GUISystem::handleEvent(const KugeEvent* event) {
  switch(event->type()) {
    case KugeEventTypes::AeroliteDestroyed:
      updateScore(static_cast<const AeroliteDestroyedEvent*>(event)->score());
      break;
    case KugeEventTypes::AeroliteSplitted:
      updateScore(static_cast<const AeroliteSplittedEvent*>(event)->score());
      break;
    default: break;
  }
}

void kuge::GUISystem::init() {
  GUIStringConfig config {};
  /* TITLE TEXT */
  config.name_   = "title";
  config.text_   = kTitleText_;
  config.color_  = ktp::Palette::white;
  config.font_   = "future";
  config.shader_ = "gui_string";
  config.vertices_ = {
     0.8f,  0.4f, 0.0f,  // top right
     0.8f, -0.4f, 0.0f,  // bottom right
    -0.8f, -0.4f, 0.0f,  // bottom left
    -0.8f,  0.4f, 0.0f   // top left
  };
  config.indices_ = {
    0, 1, 3,   // first triangle
    1, 2, 3    // second triangle
  };
  config.texture_coords_ = {
    1.0f, 0.0f,   // top right
    1.0f, 1.0f,   // bottom right
    0.0f, 1.0f,   // bottom left
    0.0f, 0.0f    // top left
  };
  title_text_ = std::make_unique<GUIStringImpl>(config);
  /* DEMO MODE TEXT */
  config.name_   = "demo";
  config.text_   = kDemoModeText_;
  config.color_  = ktp::Palette::white;
  config.font_   = "future";
  config.shader_ = "gui_string";
  config.vertices_ = {
     0.2f,  0.1f, 0.0f,  // top right
     0.2f, -0.1f, 0.0f,  // bottom right
    -0.2f, -0.1f, 0.0f,  // bottom left
    -0.2f,  0.1f, 0.0f   // top left
  };
  demo_text_ = std::make_unique<GUIStringImpl>(config);
  /* PAUSED TEXT */
  config.name_   = "paused";
  config.text_   = kPausedText_;
  config.color_  = ktp::Palette::white;
  config.font_   = "future";
  config.shader_ = "gui_string";
  config.vertices_ = {
     0.2f,  0.1f, 0.0f,  // top right
     0.2f, -0.1f, 0.0f,  // bottom right
    -0.2f, -0.1f, 0.0f,  // bottom left
    -0.2f,  0.1f, 0.0f   // top left
  };
  paused_text_ = std::make_unique<GUIStringImpl>(config);
  /* SCORE TEXT */
  config.name_   = "score";
  config.text_   = kScoreText_;
  config.color_  = ktp::Palette::white;
  config.font_   = "future";
  config.shader_ = "gui_string";
  config.vertices_ = {
    -0.80f, 0.99f, 0.0f,  // top right
    -0.80f, 0.93f, 0.0f,  // bottom right
    -1.00f, 0.93f, 0.0f,  // bottom left
    -1.00f, 0.99f, 0.0f   // top left
  };
  score_text_ = std::make_unique<GUIStringImpl>(config);
}

void kuge::GUISystem::resetScore() {
  score_ = 0;
  score_text_->updateTexture(kScoreText_ + std::to_string(score_));
}

void kuge::GUISystem::updateScore(Uint32 points) {
  score_ += points;
  score_text_->updateTexture(kScoreText_ + std::to_string(score_));
}
