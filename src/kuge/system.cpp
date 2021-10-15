#include "../include/paths.hpp"
#include "event.hpp"
#include "system.hpp"

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

void kuge::AudioSystem::handleEvent(const KugeEvent& event) {
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
  ktp::SDL2_Sound laser{};
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