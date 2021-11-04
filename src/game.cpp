#include "include/game.hpp"
#include "include/palette.hpp"
#include "include/particle.hpp"
#include "include/paths.hpp"

/* kuge/system.hpp */
const kuge::EventBus* kuge::System::event_bus_ {event_bus_};

/* kuge/event.cpp */
const ktp::SDL2_Timer& kuge::KugeEvent::gameplay_timer_ {ktp::Game::gameplay_timer_};

/* include/game_entity.hpp */
kuge::EventBus*    ktp::GameEntity::event_bus_ {nullptr};
ktp::EntitiesCount ktp::GameEntity::entities_count_ {};
ktp::EntitiesPool  ktp::GameEntity::game_entities_ {2000};

/* include/physics_component.hpp */
SDL_FPoint ktp::PhysicsComponent::b2_screen_size_ {};
b2World*   ktp::PhysicsComponent::world_ {nullptr};

/* GAME */

ktp::SDL2_Timer ktp::Game::gameplay_timer_ {};

ktp::Game::Game() {
  event_bus_.setSystems(&audio_sys_, &input_sys_, &gui_sys_, &output_sys_);
  GameEntity::event_bus_ = &event_bus_;
}

void ktp::Game::clean() {
  SDL2_Audio::closeMixer();
  SDL2_Font::closeTTF();
  SDL2_Image::closeImage();
	SDL_Quit();
}

bool ktp::Game::init() {
  SDL_LogSetAllPriority(SDL_LOG_PRIORITY_VERBOSE);
  if (!initSDL2()) return false;
  logMessage("Box2D version: " + std::to_string(b2_version.major) + '.' + std::to_string(b2_version.minor) + '.' + std::to_string(b2_version.revision));
  if (!main_window_.create(kuge::GUISystem::kTitleText_ , screen_size_)) return false;
  if (!renderer_.create(main_window_, screen_size_, SDL_RENDERER_ACCELERATED)) return false;
  if (!loadResources()) return false;
  if (!gui_sys_.init(renderer_)) return false;

  debug_draw_.setRenderer(&renderer_);
  world_.SetDebugDraw(&debug_draw_);
  debug_draw_.SetFlags(b2Draw::e_shapeBit | b2Draw::e_aabbBit | b2Draw::e_pairBit | b2Draw::e_centerOfMassBit);
  world_.SetContactListener(&contact_listener_);

  PhysicsComponent::setScreenSize({(float)screen_size_.x, (float)screen_size_.y});
  PhysicsComponent::setWorld(&world_);

  //state_ = GameState::goToState(*this, GameState::title_);
  state_ = GameState::goToState(*this, GameState::testing_);

  return true;
}

bool ktp::Game::initSDL2() {
  if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
    logSDL2Error("SDL_Init");
    return false;
  }
  if (!SDL2_Image::initImage()) return false;
  if (!SDL2_Font::initTTF()) return false;
  if (!SDL2_Audio::initMixer()) return false;

  return true;
}

bool ktp::Game::loadResources() {
  if (!audio_sys_.loadResources()) {
    return false;
  }
  ParticlesAtlas::loadTexture(renderer_);
  return true;
}

void ktp::Game::reset() {
  GameEntity::clear();
  // we need to do this to prevent some of the bodies not being destroyed
  // ie: explosion particles if you pause and go to title
  if (world_.GetBodyCount()) {
    b2Body* body {world_.GetBodyList()};
    b2Body* aux {nullptr};
    while (body) {
      aux = body->GetNext();
      world_.DestroyBody(body);
      body = aux;
    }
  }
}