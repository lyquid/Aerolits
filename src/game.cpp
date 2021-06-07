#include "include/emitter_parser.hpp"
#include "include/game.hpp"
#include "include/palette.hpp"
#include "include/resources_path.hpp"

void ktp::loadConfigFiles() {
  EmitterParser::initEmitters();
}

/* GAME */

ktp::Game::Game() {
  event_bus_.setSystems(audio_sys_, input_sys_, output_sys_);
  //emitters_.reserve(200);
}

void ktp::Game::clean() {
  ktp::SDL2_Audio::closeMixer();
  ktp::SDL2_Font::closeTTF();
  ktp::SDL2_Image::closeImage();
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

  player_.setBox2d(&world_);

  Aerolite::setB2World(&world_);
  Aerolite::setScreenSize(screen_size_);

  state_ = GameState::goToState(GameState::title_);

  paused_text_.setRenderer(renderer_);
  paused_text_.loadFromTextSolid(font_, "PAUSED", Colors::white);
  title_text_.setRenderer(renderer_);
  title_text_.loadFromTextSolid(font_, kGameTitle_, Colors::white);

  return true;
}

bool ktp::Game::initSDL2() {
  if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
    ktp::logSDL2Error("SDL_Init");
    return false;
  }
  if (!ktp::SDL2_Image::initImage()) return false;
  if (!ktp::SDL2_Font::initTTF()) return false;
  if (!ktp::SDL2_Audio::initMixer()) return false;

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
