#include "../include/palette.hpp"
#include "../include/paths.hpp"
#include "event.hpp"
#include "system.hpp"
#include <random>

/* AUDIO SYSTEM */

std::vector<ktp::SDL2_Sound>           kuge::AudioSystem::lasers_ {};
kuge::AudioSystem::laser_randomizer    kuge::AudioSystem::lasers_sequence_ {};
kuge::AudioSystem::laser_randomizer_it kuge::AudioSystem::lasers_it_ {lasers_sequence_.cbegin()};

void kuge::AudioSystem::generateRandomSequence() {
  std::random_device seed;
  std::mt19937 generator(seed());
  std::uniform_int_distribution<unsigned int> distribution(0, lasers_.size() - 1);
  for (auto& num: lasers_sequence_) {
    num = distribution(generator);
  }
}

void kuge::AudioSystem::handleEvent(const KugeEvent* event) {
  /* switch (event.getType()) {
    case EventTypes::LaserFired:
      if (lasers_[*lasers_it_].play() == -1) {
        ktp::logSDL2Error("laser_.play");
      }
      (lasers_it_ < lasers_sequence_.cend() - 1) ? ++lasers_it_ : lasers_it_ = lasers_sequence_.cbegin();
      break;

    default:
      break;
  } */
}

bool kuge::AudioSystem::loadResources() {
  ktp::SDL2_Sound laser {};
  if (!laser.loadSound(ktp::getResourcesPath("sounds") + "laser1.wav")) {
    return false;
  } else {
    lasers_.push_back(std::move(laser));
  }
  if (!laser.loadSound(ktp::getResourcesPath("sounds") + "laser2.wav")) {
    return false;
  } else {
    lasers_.push_back(std::move(laser));
  }
  if (!laser.loadSound(ktp::getResourcesPath("sounds") + "laser3.wav")) {
    return false;
  } else {
    lasers_.push_back(std::move(laser));
  }
  lasers_.shrink_to_fit();
  generateRandomSequence();
  ktp::logMessage("AudioSystem: resources loaded.");
  return true;
}

/* GUI SYSTEM */

const ktp::SDL2_Renderer* kuge::GUISystem::renderer_ {nullptr};

kuge::GUISystem& kuge::GUISystem::operator=(GUISystem&& other) noexcept {
  if (this != &other) {
    // inherited members
    event_bus_ = std::exchange(other.event_bus_, nullptr);
    // own members
    screen_size_ = std::move(other.screen_size_);
    font_        = std::move(other.font_);
    demo_text_   = std::move(other.demo_text_);
    paused_text_ = std::move(other.paused_text_);
    score_text_  = std::move(other.score_text_);
    title_text_  = std::move(other.title_text_);
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

bool kuge::GUISystem::init(const ktp::SDL2_Renderer& ren) {
  if (!font_.loadFont(ktp::getResourcesPath("fonts") + "Future n0t Found.ttf", 18)) return false;

  renderer_ = &ren; // we need this in order to create new score textures

  int w, h;

  demo_text_.texture_.loadFromTextSolid(ren, font_, kDemoModeText_, ktp::Colors::white);
  w = demo_text_.texture_.getWidth()  * screen_size_.x * 0.0025f;
  h = demo_text_.texture_.getHeight() * screen_size_.y * 0.0050f;
  demo_text_.rectangle_ = {(int)(screen_size_.x * 0.5f - w * 0.5f), (int)(screen_size_.y * 0.5f - h * 0.5f), w, h};

  paused_text_.texture_.loadFromTextSolid(ren, font_, kPausedText_, ktp::Colors::white);
  w = paused_text_.texture_.getWidth()  * screen_size_.x * 0.0025f;
  h = paused_text_.texture_.getHeight() * screen_size_.y * 0.0050f;
  paused_text_.rectangle_ = {(int)(screen_size_.x * 0.5f - w * 0.5f), (int)(screen_size_.y * 0.5f - h * 0.5f), w, h};

  score_text_.texture_.loadFromTextSolid(ren, font_, kScoreText_ + std::to_string(score_), ktp::Colors::white);
  w = score_text_.texture_.getWidth()  * screen_size_.x * 0.0009f;
  h = score_text_.texture_.getHeight() * screen_size_.y * 0.0018f;
  score_text_.rectangle_ = {0, 0, w, h};

  title_text_.texture_.loadFromTextSolid(ren, font_, kTitleText_, ktp::Colors::white);
  w = title_text_.texture_.getWidth()  * screen_size_.x * 0.0125f;
  h = title_text_.texture_.getHeight() * screen_size_.y * 0.0250f;
  title_text_.rectangle_ = {(int)(screen_size_.x * 0.5f - w * 0.5f), (int)(screen_size_.y * 0.5f - h * 0.5f), w, h};

  return true;
}

void kuge::GUISystem::resetScore() {
  score_ = 0;
  score_text_.texture_.loadFromTextSolid(*renderer_, font_, kScoreText_ + std::to_string(score_), ktp::Colors::white);
  int w = score_text_.texture_.getWidth()  * screen_size_.x * 0.0009f;
  int h = score_text_.texture_.getHeight() * screen_size_.y * 0.0018f;
  score_text_.rectangle_ = {0, 0, w, h};
}

void kuge::GUISystem::updateScore(Uint32 points) {
  score_ += points;
  score_text_.texture_.loadFromTextSolid(*renderer_, font_, kScoreText_ + std::to_string(score_), ktp::Colors::white);
  int w = score_text_.texture_.getWidth()  * screen_size_.x * 0.0009f;
  int h = score_text_.texture_.getHeight() * screen_size_.y * 0.0018f;
  score_text_.rectangle_ = {0, 0, w, h};
}