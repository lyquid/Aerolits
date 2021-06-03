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

void ktp::Game::draw() {
  renderer_.clear();

  background_.draw(renderer_);

  //renderer_.drawCross(Colors::copper_green);

  player_.draw(renderer_);
  for (const auto& emitter: emitters_) {
    emitter.draw();
  }
  for (const auto& aerolite: aerolites_) {
    aerolite.draw(renderer_);
  }

  if (debug_draw_on_) world_.DebugDraw();
  
  renderer_.present();
  ++fps_;
}

void ktp::Game::handleSDL2Events() {
  while (SDL_PollEvent(&sdl_event_)) {
    switch (sdl_event_.type) {
      case SDL_QUIT:
        input_sys_.postEvent(kuge::EventTypes::ExitGame);
        quit_ = true;
        break;
      case SDL_KEYDOWN:
        // input_sys_.postEvent(kuge::EventTypes::KeyPressed, SDL_GetKeyName(sdl_event_.key.keysym.sym));
        handleSDL2KeyEvents(sdl_event_.key.keysym.sym);
        break;
      case SDL_MOUSEBUTTONDOWN: {
        int x{0}, y{0};
        if (SDL_GetMouseState(&x, &y) & SDL_BUTTON(SDL_BUTTON_LEFT)) {
          Emitter emi{"plasma", {static_cast<float>(x), static_cast<float>(y)}};
          // emitters_.push_back(std::move({EmitterTypes::Fire, {static_cast<float>(x), static_cast<float>(y)}}));
          emitters_.push_back(std::move(emi));
        }
        break;
      }
      default:
        break;
    }
  }
}

void ktp::Game::handleSDL2KeyEvents(const SDL_Keycode& key) {
  switch (key) {
    case SDLK_ESCAPE:
      input_sys_.postEvent(kuge::EventTypes::ExitGame);
      quit_ = true;
      break;
    case SDLK_F1:
      debug_draw_on_ = !debug_draw_on_;
      break;
    default:
      break;
  }
}

bool ktp::Game::init() {
  if (!initSDL2()) return false;
  logMessage("Box2D version: " + std::to_string(b2_version.major) + '.' + std::to_string(b2_version.minor) + '.' + std::to_string(b2_version.revision));
  if (!main_window_.create(kGameTitle_, screen_size_)) return false;
  if (!renderer_.create(main_window_, screen_size_, SDL_RENDERER_ACCELERATED)) return false;
  if (!loadResources()) return false;
  fps_.start();

  debug_draw_.setRenderer(&renderer_);
  world_.SetDebugDraw(&debug_draw_);
  debug_draw_.SetFlags(b2Draw::e_shapeBit | b2Draw::e_aabbBit | b2Draw::e_pairBit | b2Draw::e_centerOfMassBit);

  player_.setBox2d(&world_);

  Aerolite::setB2World(&world_);
  Aerolite::setScreenSize(screen_size_);

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

void ktp::Game::update(float delta_time) {
  /* FPS */
  main_window_.setTitle(kGameTitle_ + " - FPS: " + std::to_string(fps_.average()));
  /* Box2D */
  world_.Step(delta_time, velocity_iterations_, position_iterations_);
  /* Background */
  background_.update(delta_time);
  /* Player */
  player_.update(delta_time);
  /* Emitters */
  auto iter = emitters_.begin();
  while (iter != emitters_.end()) {
    if (iter->canBeDeleted()) {
      iter = emitters_.erase(iter);
    } else {
      iter->generateParticles();
      iter->update();
      ++iter;
    }
  }
  /* Aerolites */
  auto aerolite = aerolites_.begin();
  while (aerolite != aerolites_.end()) {
    if (aerolite->canBeDeleted()) {
      aerolite = aerolites_.erase(aerolite);
    } else {
      aerolite->update();
      ++aerolite;
    }
  }
  if (aerolites_.size() < 4) {
    aerolites_.push_back(Aerolite::spawnAerolite());
  }
  /* Event bus */
  event_bus_.processEvents();
}