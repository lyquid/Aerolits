#include "include/game.hpp"
#include "include/game_state.hpp"
#include "include/physics_component.hpp"
#include "kuge/system.hpp" // GUISystem
#include <SDL.h>
#include <memory>
#include <string> // std::to_string
#include <utility> // std::move

ktp::DemoState    ktp::GameState::demo_ {};
ktp::PausedState  ktp::GameState::paused_ {};
ktp::PlayingState ktp::GameState::playing_ {};
ktp::TestingState ktp::GameState::testing_ {};
ktp::TitleState   ktp::GameState::title_ {};

void ktp::GameState::setWindowTitle(Game& game) {
  game.main_window_.setTitle(
    kuge::GUISystem::kTitleText_
    + " | Frame time: " + std::to_string((int)(game.frame_time_ * 1000)) + "ms."
    + " | b2Bodies: " + std::to_string(game.world_.GetBodyCount())
    + " | Entities: " + std::to_string(GameEntity::count()) + '/' + std::to_string(GameEntity::game_entities_.capacity())
    + " (Player: " + std::to_string(GameEntity::entitiesCount(EntityTypes::Player) + GameEntity::entitiesCount(EntityTypes::PlayerDemo))
    + " Aerolites: " + std::to_string(GameEntity::entitiesCount(EntityTypes::Aerolite))
    + " Projectiles: " + std::to_string(GameEntity::entitiesCount(EntityTypes::Projectile))
    + " Explosions: " + std::to_string(GameEntity::entitiesCount(EntityTypes::ExplosionParticle)) + ')'
  );
}

/* DEMO STATE */

void ktp::DemoState::draw(Game& game) {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  for (auto i = 0u; i < GameEntity::game_entities_.capacity(); ++i) {
    if (GameEntity::game_entities_[i].active_) {
      GameEntity::game_entities_[i].object_.draw();
    }
  }

  game.gui_sys_.scoreText()->draw();

  if (blink_flag_) game.gui_sys_.demoText()->draw();

  if (SDL2_Timer::SDL2Ticks() - blink_timer_ > 500) {
    blink_flag_ = !blink_flag_;
    blink_timer_ = SDL2_Timer::SDL2Ticks();
  }

  if (game.debug_draw_on_) game.world_.DebugDraw();

  SDL_GL_SwapWindow(game.main_window_.getWindow());
}

ktp::GameState* ktp::DemoState::enter(Game& game) {
  Game::gameplay_timer_.paused() ? Game::gameplay_timer_.resume() : Game::gameplay_timer_.start();
  for (auto i = 0u; i < GameEntity::game_entities_.capacity(); ++i) {
    if (GameEntity::game_entities_[i].object_.type() == EntityTypes::Player) {
      GameEntity::game_entities_[i].object_.free(i);
    }
  }
  GameEntity::createEntity(EntityTypes::PlayerDemo);
  blink_flag_ = true;
  blink_timer_ = SDL2_Timer::SDL2Ticks();
  return this;
}

void ktp::DemoState::handleEvents(Game& game) {
  while (SDL_PollEvent(&sdl_event_)) {
    switch (sdl_event_.type) {
      case SDL_QUIT:
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
      game.quit_ = true;
      break;
    case SDLK_F1:
      game.debug_draw_on_ = !game.debug_draw_on_;
      break;
    case SDLK_F2:
      wireframe_mode_ = !wireframe_mode_;
      wireframe_mode_ ? glPolygonMode(GL_FRONT_AND_BACK, GL_LINE) : glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
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

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  for (auto i = 0u; i < GameEntity::game_entities_.capacity(); ++i) {
    if (GameEntity::game_entities_[i].active_) {
      GameEntity::game_entities_[i].object_.draw();
    }
  }

  game.gui_sys_.scoreText()->draw();

  if (blink_flag_) game.gui_sys_.pausedText()->draw();

  if (SDL2_Timer::SDL2Ticks() - blink_timer_ > 500) {
    blink_flag_ = !blink_flag_;
    blink_timer_ = SDL2_Timer::SDL2Ticks();
  }

  if (game.debug_draw_on_) game.world_.DebugDraw();

  SDL_GL_SwapWindow(game.main_window_.getWindow());
}

ktp::GameState* ktp::PausedState::enter(Game& game) {
  Game::gameplay_timer_.pause();
  blink_flag_ = true;
  blink_timer_ = SDL2_Timer::SDL2Ticks();
  return this;
}

void ktp::PausedState::handleEvents(Game& game) {
  while (SDL_PollEvent(&sdl_event_)) {
    switch (sdl_event_.type) {
      case SDL_QUIT:
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
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  for (auto i = 0u; i < GameEntity::game_entities_.capacity(); ++i) {
    if (GameEntity::game_entities_[i].active_) {
      GameEntity::game_entities_[i].object_.draw();
    }
  }

  game.gui_sys_.scoreText()->draw();

  if (game.debug_draw_on_) game.world_.DebugDraw();

  SDL_GL_SwapWindow(game.main_window_.getWindow());
}

ktp::GameState* ktp::PlayingState::enter(Game& game) {
  Game::gameplay_timer_.paused() ? Game::gameplay_timer_.resume() : Game::gameplay_timer_.start();
  return this;
}

void ktp::PlayingState::handleEvents(Game& game) {
  while (SDL_PollEvent(&sdl_event_)) {
    switch (sdl_event_.type) {
      case SDL_QUIT:
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
    case SDLK_F2:
      wireframe_mode_ = !wireframe_mode_;
      wireframe_mode_ ? glPolygonMode(GL_FRONT_AND_BACK, GL_LINE) : glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
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
  for (std::size_t i = 0; i < GameEntity::game_entities_.capacity(); ++i) {
    if (GameEntity::game_entities_[i].active_) {
      if (GameEntity::game_entities_[i].object_.canBeDeactivated()) {
        GameEntity::game_entities_[i].object_.free(i);
      } else {
        GameEntity::game_entities_[i].object_.update(delta_time);
      }
    }
  }
  if (GameEntity::entitiesCount(EntityTypes::Aerolite) < 4) AerolitePhysicsComponent::spawnMovingAerolite();

  game.event_bus_.processEvents();
}

/* TESTING STATE */

void ktp::TestingState::draw(Game& game) {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  for (auto i = 0u; i < GameEntity::game_entities_.capacity(); ++i) {
    if (GameEntity::game_entities_[i].active_) {
      GameEntity::game_entities_[i].object_.draw();
    }
  }

  test_->draw();

  if (game.debug_draw_on_) game.world_.DebugDraw();

  SDL_GL_SwapWindow(game.main_window_.getWindow());
}

ktp::GameState* ktp::TestingState::enter(Game& game) {
  game.reset();
  Game::gameplay_timer_.paused() ? Game::gameplay_timer_.resume() : Game::gameplay_timer_.start();
  // GameEntity::createEntity(EntityTypes::Player);
  test_ = new Testing;
  test_->init();
  return this;
}

void ktp::TestingState::handleEvents(Game& game) {
  while (SDL_PollEvent(&sdl_event_)) {
    switch (sdl_event_.type) {
      case SDL_QUIT:
        game.quit_ = true;
        break;
      case SDL_KEYDOWN:
        handleSDL2KeyEvents(game, sdl_event_.key.keysym.sym);
        break;
      case SDL_MOUSEBUTTONDOWN: {
        int x{0}, y{0};
        if (SDL_GetMouseState(&x, &y) & SDL_BUTTON(SDL_BUTTON_LEFT)) {
          AerolitePhysicsComponent::spawnAerolite({(float)x, (float)y});
          logMessage("clicked " + std::to_string(x) + ", " + std::to_string(y));
        }
        break;
      }
      default: break;
    }
  }
}

void ktp::TestingState::handleSDL2KeyEvents(Game& game, SDL_Keycode key) {
  switch (key) {
    case SDLK_ESCAPE:
      game.quit_ = true;
      break;
    case SDLK_F1:
      game.debug_draw_on_ = !game.debug_draw_on_;
      break;
    case SDLK_F2:
      wireframe_mode_ = !wireframe_mode_;
      wireframe_mode_ ? glPolygonMode(GL_FRONT_AND_BACK, GL_LINE) : glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
      break;
    case SDLK_SPACE:
      break;
    case SDLK_r:
      game.state_ = goToState(game, GameState::testing_);
      break;
    default:
      break;
  }
}

void ktp::TestingState::update(Game& game, float delta_time) {
  // Window title
  setWindowTitle(game);
  // Box2D
  game.world_.Step(delta_time, game.velocity_iterations_, game.position_iterations_);
  // Entities
  for (std::size_t i = 0; i < GameEntity::game_entities_.capacity(); ++i) {
    if (GameEntity::game_entities_[i].active_) {
      if (GameEntity::game_entities_[i].object_.canBeDeactivated()) {
        GameEntity::game_entities_[i].object_.free(i);
      } else {
        GameEntity::game_entities_[i].object_.update(delta_time);
      }
    }
  }
  test_->update(delta_time);
  // if (GameEntity::entitiesCount(EntityTypes::Aerolite) < 1) AerolitePhysicsComponent::spawnMovingAerolite();
  game.event_bus_.processEvents();
}

/* TITLE STATE */

void ktp::TitleState::draw(Game& game) {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // for (auto i = 0u; i < GameEntity::game_entities_.capacity(); ++i) {
  //   if (GameEntity::game_entities_[i].object_.type() == EntityTypes::Background) {
  //     GameEntity::game_entities_[i].object_.draw(game.renderer_);
  //     break;
  //   }
  // }

  game.gui_sys_.titleText()->draw();

  SDL_GL_SwapWindow(game.main_window_.getWindow());
}

ktp::GameState* ktp::TitleState::enter(Game& game) {
  game.reset();
  // GameEntity::createEntity(EntityTypes::Background);
  GameEntity::createEntity(EntityTypes::Player);
  demo_time_ = SDL2_Timer::SDL2Ticks();
  game.gui_sys_.resetScore();
  return this;
}

void ktp::TitleState::handleEvents(Game& game) {
  while (SDL_PollEvent(&sdl_event_)) {
    switch (sdl_event_.type) {
      case SDL_QUIT:
        game.quit_ = true;
        break;
      case SDL_KEYDOWN:
        // input_sys_.postEvent(kuge::EventTypes::KeyPressed, SDL_GetKeyName(sdl_event_.key.keysym.sym));
        handleSDL2KeyEvents(game, sdl_event_.key.keysym.sym);
        break;
      case SDL_MOUSEBUTTONDOWN:
        demo_time_ = SDL2_Timer::SDL2Ticks();
        break;
      default:
        break;
    }
  }
}

void ktp::TitleState::handleSDL2KeyEvents(Game& game, SDL_Keycode key) {
  switch (key) {
    case SDLK_ESCAPE:
      game.quit_ = true;
      break;
    default:
      demo_time_ = SDL2_Timer::SDL2Ticks();
      game.state_ = goToState(game, GameState::playing_);
      break;
  }
}

void ktp::TitleState::update(Game& game, float delta_time) {
  // Window title
  setWindowTitle(game);
  // Background
  for (auto i = 0u; i < GameEntity::game_entities_.capacity(); ++i) {
    // GameEntity::game_entities_[i].object_.update(delta_time * kDefaultBackgroundDeltaInMenu_);
  }
  // enter Demo mode
  if (SDL2_Timer::SDL2Ticks() - demo_time_ > kWaitForDemo_) {
    game.state_ = goToState(game, GameState::demo_);
  }
}
