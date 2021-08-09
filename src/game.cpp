#include "include/emitter_parser.hpp"
#include "include/game.hpp"
#include "include/palette.hpp"
#include "include/particle.hpp"
#include "include/resources_path.hpp"

void ktp::loadConfigFiles() {
  EmitterParser::initEmitters();
}

ktp::EntitiesCount ktp::GameEntity::entities_count_ {};
ktp::EntitiesPool  ktp::GameEntity::game_entities_ {40};

SDL_FPoint ktp::PhysicsComponent::b2_screen_size_ {};
b2World*   ktp::PhysicsComponent::world_ {nullptr};

/* GAME */

ktp::Game::Game() {
  event_bus_.setSystems(audio_sys_, input_sys_, output_sys_);
}

void ktp::Game::clean() {
  SDL2_Audio::closeMixer();
  SDL2_Font::closeTTF();
  SDL2_Image::closeImage();
	SDL_Quit();
}

bool ktp::Game::init() {
  if (!initSDL2()) return false;
  logMessage("Box2D version: " + std::to_string(b2_version.major) + '.' + std::to_string(b2_version.minor) + '.' + std::to_string(b2_version.revision));
  if (!main_window_.create(kGameTitle_, screen_size_)) return false;
  if (!renderer_.create(main_window_, screen_size_, SDL_RENDERER_ACCELERATED)) return false;
  if (!loadResources()) return false;

  debug_draw_.setRenderer(&renderer_);
  world_.SetDebugDraw(&debug_draw_);
  debug_draw_.SetFlags(b2Draw::e_shapeBit | b2Draw::e_aabbBit | b2Draw::e_pairBit | b2Draw::e_centerOfMassBit);
  world_.SetContactListener(&contact_listener_);

  PhysicsComponent::setScreenSize({(float)screen_size_.x, (float)screen_size_.y});
  PhysicsComponent::setWorld(&world_);

  paused_text_.setRenderer(renderer_);
  paused_text_.loadFromTextSolid(font_, "PAUSED", Colors::white);
  title_text_.setRenderer(renderer_);
  title_text_.loadFromTextSolid(font_, kGameTitle_, Colors::white);
  demo_text_.setRenderer(renderer_);
  demo_text_.loadFromTextSolid(font_, "DEMO MODE", Colors::white);

  state_ = GameState::goToState(*this, GameState::title_);

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
  if (!font_.loadFont(getResourcesPath("fonts") + "Future n0t Found.ttf", 18)) {
    return false;
  }
  if (!audio_sys_.loadResources()) {
    return false;
  }
  ParticlesAtlas::loadTexture(renderer_);
  return true;
}

void ktp::Game::reset() {
  GameEntity::clear();
  // we need to do this to prevent some of the bodies not being destroyed
  // ie: explosion particles if you pause and got to title
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