#include "include/game.hpp"
#include "include/game_state.hpp"
#include "include/physics_component.hpp"
#include <SDL.h>
#include <memory>
#include <string> // std::to_string
#include <utility> // std::move

ktp::DemoState    ktp::GameState::demo_ {};
ktp::PausedState  ktp::GameState::paused_ {};
ktp::PlayingState ktp::GameState::playing_ {};
ktp::TitleState   ktp::GameState::title_ {};

void ktp::GameState::setWindowTitle(Game& game) {
  game.main_window_.setTitle(
    game.kGameTitle_
    + " | Frame time: " + std::to_string((int)(game.frame_time_ * 1000)) + "ms."
    + " | GameEntities: " + std::to_string(GameEntity::count())
    + " | b2Bodies: " + std::to_string(game.world_.GetBodyCount())
  );
}

/* DEMO STATE */

void ktp::DemoState::draw(Game& game) {
  game.renderer_.clear();

  for (const auto& entity: GameEntity::game_entities_) {
    entity.draw(game.renderer_);
  }

  if (game.debug_draw_on_) game.world_.DebugDraw();

  if (blink_flag_) {
    const int w = game.screen_size_.x * 0.2f;
    const int h = game.screen_size_.y * 0.1f;
    game.demo_text_.render({(int)(game.screen_size_.x * 0.5f - w * 0.5f), (int)(game.screen_size_.y * 0.5f - h * 0.5f), w, h});
  }
  if (SDL2_Timer::getSDL2Ticks() - blink_timer_ > 500) {
    blink_flag_ = !blink_flag_;
    blink_timer_ = SDL2_Timer::getSDL2Ticks();
  }

  game.renderer_.present();
}

ktp::GameState* ktp::DemoState::enter(Game& game) {
  GameEntity::game_entities_.remove_if(GameEntity::isPlayer);
  GameEntity::createEntity(EntityTypes::PlayerDemo);
  blink_flag_ = true;
  blink_timer_ = SDL2_Timer::getSDL2Ticks();
  return this;
}

void ktp::DemoState::handleEvents(Game& game) {
  while (SDL_PollEvent(&sdl_event_)) {
    switch (sdl_event_.type) {
      case SDL_QUIT:
        game.input_sys_.postEvent(kuge::EventTypes::ExitGame);
        game.quit_ = true;
        break;
      case SDL_KEYDOWN:
        handleSDL2KeyEvents(game, sdl_event_.key.keysym.sym);
        break;
      case SDL_MOUSEBUTTONDOWN:
        game.state_ = goToState(game, GameState::title_);
        break;
      default:
        break;
    }
  }
}

void ktp::DemoState::handleSDL2KeyEvents(Game& game, SDL_Keycode key) {
  switch (key) {
    case SDLK_ESCAPE:
      game.input_sys_.postEvent(kuge::EventTypes::ExitGame);
      game.quit_ = true;
      break;
    case SDLK_F1:
      game.debug_draw_on_ = !game.debug_draw_on_;
      break;
    default:
      game.state_ = goToState(game, GameState::title_);
      break;
  }
}

void ktp::DemoState::update(Game& game, float delta_time) {
  playing_.update(game, delta_time);
}

/* PAUSED STATE */

void ktp::PausedState::draw(Game& game) {
  game.renderer_.clear();

  for (const auto& entity: GameEntity::game_entities_) {
    entity.draw(game.renderer_);
  }

  if (game.debug_draw_on_) game.world_.DebugDraw();

  if (blink_flag_) {
    const int w = game.screen_size_.x * 0.1f;
    const int h = game.screen_size_.y * 0.05f;
    game.paused_text_.render({(int)(game.screen_size_.x * 0.5f - w * 0.5f), (int)(game.screen_size_.y * 0.5f - h * 0.5f), w, h});
  }
  if (SDL2_Timer::getSDL2Ticks() - blink_timer_ > 500) {
    blink_flag_ = !blink_flag_;
    blink_timer_ = SDL2_Timer::getSDL2Ticks();
  }

  game.renderer_.present();
}

ktp::GameState* ktp::PausedState::enter(Game& game) {
  blink_flag_ = true;
  blink_timer_ = SDL2_Timer::getSDL2Ticks();
  return this;
}

void ktp::PausedState::handleEvents(Game& game) {
  while (SDL_PollEvent(&sdl_event_)) {
    switch (sdl_event_.type) {
      case SDL_QUIT:
        game.input_sys_.postEvent(kuge::EventTypes::ExitGame);
        game.quit_ = true;
        break;
      case SDL_KEYDOWN:
        handleSDL2KeyEvents(game, sdl_event_.key.keysym.sym);
        break;
      default:
        break;
    }
  }
}

void ktp::PausedState::handleSDL2KeyEvents(Game& game, SDL_Keycode key) {
  switch (key) {
    case SDLK_ESCAPE:
      game.state_ = goToState(game, GameState::title_);
      break;
    case SDLK_F1:
      game.debug_draw_on_ = !game.debug_draw_on_;
      break;
    default:
      game.state_ = goToState(game, GameState::playing_);
      break;
  }
}

void ktp::PausedState::update(Game& game, float delta_time) {
  // Window title
  setWindowTitle(game);
}

/* PLAYING STATE */

void ktp::PlayingState::draw(Game& game) {
  game.renderer_.clear();

  for (const auto& entity: GameEntity::game_entities_) {
    entity.draw(game.renderer_);
  }

  if (game.debug_draw_on_) game.world_.DebugDraw();

  game.renderer_.present();
}

void ktp::PlayingState::handleEvents(Game& game) {
  while (SDL_PollEvent(&sdl_event_)) {
    switch (sdl_event_.type) {
      case SDL_QUIT:
        game.input_sys_.postEvent(kuge::EventTypes::ExitGame);
        game.quit_ = true;
        break;
      case SDL_KEYDOWN:
        // input_sys_.postEvent(kuge::EventTypes::KeyPressed, SDL_GetKeyName(sdl_event_.key.keysym.sym));
        handleSDL2KeyEvents(game, sdl_event_.key.keysym.sym);
        break;
      case SDL_MOUSEBUTTONDOWN: {
        int x{0}, y{0};
        if (SDL_GetMouseState(&x, &y) & SDL_BUTTON(SDL_BUTTON_LEFT)) {
          // Emitter emi{"plasma", {static_cast<float>(x), static_cast<float>(y)}};
          // emitters_.push_back(std::move({EmitterTypes::Fire, {static_cast<float>(x), static_cast<float>(y)}}));
          // game.emitters_.push_back(std::move(emi));
        }
        break;
      }
      default:
        break;
    }
  }
}

void ktp::PlayingState::handleSDL2KeyEvents(Game& game, SDL_Keycode key) {
  switch (key) {
    case SDLK_ESCAPE:
      game.state_ = goToState(game, GameState::paused_);
      break;
    case SDLK_F1:
      game.debug_draw_on_ = !game.debug_draw_on_;
      break;
    case SDLK_p:
      game.state_ = goToState(game, GameState::paused_);
      break;
    default:
      break;
  }
}

void ktp::PlayingState::update(Game& game, float delta_time) {
  // Window title
  setWindowTitle(game);
  // Box2D
  game.world_.Step(delta_time, game.velocity_iterations_, game.position_iterations_);
  // Entities
  auto entity = GameEntity::game_entities_.begin();
  while (entity != GameEntity::game_entities_.end()) {
    if (entity->physics_->canBeDeleted()) {
      entity = GameEntity::game_entities_.erase(entity);
    } else {
      entity->update(delta_time);
      ++entity;
    }
  }
  if (GameEntity::aerolite_count_ < 4) AerolitePhysicsComponent::spawnAerolite();
  // Event bus
  game.event_bus_.processEvents();
}

/* TITLE STATE */

void ktp::TitleState::draw(Game& game) {
  game.renderer_.clear();

  for (const auto& entity: GameEntity::game_entities_) {
    if (entity.type() == EntityTypes::Background) {
      entity.draw(game.renderer_);
      break;
    }
  }

  const int w = game.screen_size_.x * 0.75f;
  const int h = game.screen_size_.y * 0.50f;
  game.title_text_.render({(int)(game.screen_size_.x * 0.5f - w * 0.5f), (int)(game.screen_size_.y * 0.5f - h * 0.5f), w, h});

  game.renderer_.present();
}

ktp::GameState* ktp::TitleState::enter(Game& game) {
  game.reset();
  GameEntity::createEntity(EntityTypes::Background);
  GameEntity::createEntity(EntityTypes::Player);
  demo_time_ = SDL2_Timer::getSDL2Ticks();
  return this;
}

void ktp::TitleState::handleEvents(Game& game) {
  while (SDL_PollEvent(&sdl_event_)) {
    switch (sdl_event_.type) {
      case SDL_QUIT:
        game.input_sys_.postEvent(kuge::EventTypes::ExitGame);
        game.quit_ = true;
        break;
      case SDL_KEYDOWN:
        // input_sys_.postEvent(kuge::EventTypes::KeyPressed, SDL_GetKeyName(sdl_event_.key.keysym.sym));
        handleSDL2KeyEvents(game, sdl_event_.key.keysym.sym);
        break;
      case SDL_MOUSEBUTTONDOWN:
        demo_time_ = SDL2_Timer::getSDL2Ticks();
        break;
      default:
        break;
    }
  }
}

void ktp::TitleState::handleSDL2KeyEvents(Game& game, SDL_Keycode key) {
  switch (key) {
    case SDLK_ESCAPE:
      game.input_sys_.postEvent(kuge::EventTypes::ExitGame);
      game.quit_ = true;
      break;
    default:
      demo_time_ = SDL2_Timer::getSDL2Ticks();
      game.state_ = goToState(game, GameState::playing_);
      break;
  }
}

void ktp::TitleState::update(Game& game, float delta_time) {
  // Window title
  setWindowTitle(game);
  // Background
  for (auto& entity: GameEntity::game_entities_) {
    entity.update(delta_time * kDefaultBackgroundDeltaInMenu_);
  }
  // enter Demo mode
  if (SDL2_Timer::getSDL2Ticks() - demo_time_ > kWaitForDemo_) {
    game.state_ = goToState(game, GameState::demo_);
  }
}