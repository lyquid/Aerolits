#include "include/debug_draw.hpp"
#include "include/game.hpp"
#include "include/game_entity.hpp"
#include "include/game_state.hpp"
#include "include/physics_component.hpp"
#include "include/testing.hpp"
#include "kuge/backend_system.hpp"
#include "kuge/system.hpp" // GUISystem
#include "sdl2_wrappers/sdl2_timer.hpp"
#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"
#include <SDL.h>
#include <memory>
#include <string> // std::to_string
#include <utility> // std::move

ktp::DemoState    ktp::GameState::demo_ {};
ktp::PausedState  ktp::GameState::paused_ {};
ktp::PlayingState ktp::GameState::playing_ {};
ktp::TestingState ktp::GameState::testing_ {};
ktp::TitleState   ktp::GameState::title_ {};

ktp::DebugDraw ktp::GameState::b2_debug_ {};
bool ktp::GameState::backend_draw_ {false};
bool ktp::GameState::culling_ {false};
bool ktp::GameState::debug_draw_ {false};
bool ktp::GameState::deep_test_ {false};
bool ktp::GameState::polygon_draw_ {false};

void ktp::GameState::setDebugDrawFlags(const kuge::B2DebugFlags& debug_flags) {
  Uint32 final_flags {};
  if (debug_flags.aabb) {
    final_flags |= b2Draw::e_aabbBit;
  }
  if (debug_flags.center) {
    final_flags |= b2Draw::e_centerOfMassBit;
  }
  if (debug_flags.joints) {
    final_flags |= b2Draw::e_jointBit;
  }
  if (debug_flags.pairs) {
    final_flags |= b2Draw::e_pairBit;
  }
  if (debug_flags.shapes) {
    final_flags |= b2Draw::e_shapeBit;
  }
  b2_debug_.SetFlags(final_flags);
}

/* DEMO STATE */

void ktp::DemoState::draw(Game& game) {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  for (auto i = 0u; i <= GameEntity::game_entities_.highestActiveIndex(); ++i) {
    if (GameEntity::game_entities_.active(i)) {
      GameEntity::game_entities_[i].draw();
    }
  }

  game.gui_sys_.scoreText()->draw();

  if (blink_flag_) game.gui_sys_.demoText()->draw();

  if (SDL2_Timer::SDL2Ticks() - blink_timer_ > 500) {
    blink_flag_ = !blink_flag_;
    blink_timer_ = SDL2_Timer::SDL2Ticks();
  }

  if (debug_draw_) {
    Game::b2_world_.DebugDraw();
    b2_debug_.Draw();
  }

  if (backend_draw_) game.backend_sys_.draw();

  SDL_GL_SwapWindow(game.main_window_.getWindow());
}

ktp::GameState* ktp::DemoState::enter(Game& game) {
  Game::gameplay_timer_.paused() ? Game::gameplay_timer_.resume() : Game::gameplay_timer_.start();
  for (auto i = 0u; i < GameEntity::game_entities_.highestActiveIndex(); ++i) {
    if (GameEntity::game_entities_[i].type() == EntityTypes::Player) {
      GameEntity::game_entities_[i].free(i);
    }
  }
  GameEntity::createEntity(EntityTypes::PlayerDemo);
  blink_flag_ = true;
  blink_timer_ = SDL2_Timer::SDL2Ticks();
  return this;
}

void ktp::DemoState::handleEvents(Game& game) {
  while (SDL_PollEvent(&sdl_event_)) {
    ImGui_ImplSDL2_ProcessEvent(&sdl_event_);
    switch (sdl_event_.type) {
      case SDL_QUIT:
        game.quit_ = true;
        break;
      case SDL_KEYDOWN:
        if (!ImGui::GetIO().WantCaptureKeyboard) handleSDL2KeyEvents(game, sdl_event_.key.keysym.sym);
        break;
      case SDL_MOUSEBUTTONDOWN:
        if (!ImGui::GetIO().WantCaptureMouse) game.state_ = goToState(game, GameState::title_);
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
      backend_draw_ = !backend_draw_;
      break;
    case SDLK_F2:
      polygon_draw_ = !polygon_draw_;
      updatePolygonDraw();
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

  for (auto i = 0u; i <= GameEntity::game_entities_.highestActiveIndex(); ++i) {
    if (GameEntity::game_entities_.active(i)) {
      GameEntity::game_entities_[i].draw();
    }
  }

  game.gui_sys_.scoreText()->draw();

  if (blink_flag_) game.gui_sys_.pausedText()->draw();

  if (SDL2_Timer::SDL2Ticks() - blink_timer_ > 500) {
    blink_flag_ = !blink_flag_;
    blink_timer_ = SDL2_Timer::SDL2Ticks();
  }

  if (debug_draw_) {
    Game::b2_world_.DebugDraw();
    b2_debug_.Draw();
  }

  if (backend_draw_) game.backend_sys_.draw();

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
    ImGui_ImplSDL2_ProcessEvent(&sdl_event_);
    switch (sdl_event_.type) {
      case SDL_QUIT:
        game.quit_ = true;
        break;
      case SDL_KEYDOWN:
        if (!ImGui::GetIO().WantCaptureKeyboard) handleSDL2KeyEvents(game, sdl_event_.key.keysym.sym);
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
      backend_draw_ = !backend_draw_;
      break;
    case SDLK_F2:
      polygon_draw_ = !polygon_draw_;
      updatePolygonDraw();
      break;
    default:
      game.state_ = goToState(game, GameState::playing_);
      break;
  }
}

void ktp::PausedState::update(Game& game, float delta_time) {}

/* PLAYING STATE */

void ktp::PlayingState::draw(Game& game) {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  for (auto i = 0u; i <= GameEntity::game_entities_.highestActiveIndex(); ++i) {
    if (GameEntity::game_entities_.active(i)) {
      GameEntity::game_entities_[i].draw();
    }
  }

  game.gui_sys_.scoreText()->draw();

  if (debug_draw_) {
    Game::b2_world_.DebugDraw();
    b2_debug_.Draw();
  }

  if (backend_draw_) game.backend_sys_.draw();

  SDL_GL_SwapWindow(game.main_window_.getWindow());
}

ktp::GameState* ktp::PlayingState::enter(Game& game) {
  Game::gameplay_timer_.paused() ? Game::gameplay_timer_.resume() : Game::gameplay_timer_.start();
  return this;
}

void ktp::PlayingState::handleEvents(Game& game) {
  while (SDL_PollEvent(&sdl_event_)) {
    ImGui_ImplSDL2_ProcessEvent(&sdl_event_);
    switch (sdl_event_.type) {
      case SDL_QUIT:
        game.quit_ = true;
        break;
      case SDL_KEYDOWN:
        if (!ImGui::GetIO().WantCaptureKeyboard) handleSDL2KeyEvents(game, sdl_event_.key.keysym.sym);
        break;
      case SDL_MOUSEBUTTONDOWN: {
        if (!ImGui::GetIO().WantCaptureMouse) {
          int x{0}, y{0};
          if (SDL_GetMouseState(&x, &y) & SDL_BUTTON(SDL_BUTTON_LEFT)) {
            // SDL_SetRelativeMouseMode(SDL_TRUE);
            AerolitePhysicsComponent::spawnAerolite({(float)x, (float)game.screen_size_.y - (float)y});
            logMessage("clicked " + std::to_string(x) + ", " + std::to_string(game.screen_size_.y - y));
          } else if (SDL_GetMouseState(&x, &y) & SDL_BUTTON(SDL_BUTTON_RIGHT)) {
            SDL_SetRelativeMouseMode(SDL_FALSE);
          }
        }
        break;
      }
      // case SDL_MOUSEBUTTONDOWN: {
      //   int x{0}, y{0};
      //   if (SDL_GetMouseState(&x, &y) & SDL_BUTTON(SDL_BUTTON_LEFT)) {
      //     // Emitter emi{"plasma", {static_cast<float>(x), static_cast<float>(y)}};
      //     // emitters_.push_back(std::move({EmitterTypes::Fire, {static_cast<float>(x), static_cast<float>(y)}}));
      //     // game.emitters_.push_back(std::move(emi));
      //   }
      //   break;
      // }
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
      backend_draw_ = !backend_draw_;
      break;
    case SDLK_F2:
      polygon_draw_ = !polygon_draw_;
      updatePolygonDraw();
      break;
    case SDLK_p:
      game.state_ = goToState(game, GameState::paused_);
      break;
    default:
      break;
  }
}

void ktp::PlayingState::update(Game& game, float delta_time) {
  // Box2D
  Game::b2_world_.Step(delta_time, game.velocity_iterations_, game.position_iterations_);
  // Entities
  for (auto i = 0u; i <= GameEntity::game_entities_.highestActiveIndex(); ++i) {
    if (GameEntity::game_entities_.active(i)) {
      if (GameEntity::game_entities_[i].canBeDeactivated()) {
        GameEntity::game_entities_[i].free(i);
      } else {
        GameEntity::game_entities_[i].update(delta_time);
      }
    }
  }
  if (GameEntity::entitiesCount(EntityTypes::Aerolite) < 4) AerolitePhysicsComponent::spawnMovingAerolite();

  game.event_bus_.processEvents();
}

/* TESTING STATE */

void ktp::TestingState::draw(Game& game) {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  for (auto i = 0u; i <= GameEntity::game_entities_.highestActiveIndex(); ++i) {
    if (GameEntity::game_entities_.active(i)) {
      GameEntity::game_entities_[i].draw();
    }
  }

  test_->draw();

  if (debug_draw_) {
    Game::b2_world_.DebugDraw();
    b2_debug_.Draw();
  }

  if (backend_draw_) game.backend_sys_.draw();

  SDL_GL_SwapWindow(game.main_window_.getWindow());
}

ktp::GameState* ktp::TestingState::enter(Game& game) {
  game.reset();
  Game::gameplay_timer_.paused() ? Game::gameplay_timer_.resume() : Game::gameplay_timer_.start();
  delete test_;
  test_ = new Testing;
  test_->init();
  return this;
}

void ktp::TestingState::handleEvents(Game& game) {
  while (SDL_PollEvent(&sdl_event_)) {
    ImGui_ImplSDL2_ProcessEvent(&sdl_event_);
    switch (sdl_event_.type) {
      case SDL_QUIT:
        game.quit_ = true;
        break;
      case SDL_KEYDOWN:
        if (!ImGui::GetIO().WantCaptureKeyboard) handleSDL2KeyEvents(game, sdl_event_.key.keysym.sym);
        break;
      case SDL_MOUSEBUTTONDOWN: {
        if (!ImGui::GetIO().WantCaptureMouse) {
          int x{0}, y{0};
          if (SDL_GetMouseState(&x, &y) & SDL_BUTTON(SDL_BUTTON_LEFT)) {
            SDL_SetRelativeMouseMode(SDL_TRUE);
            // AerolitePhysicsComponent::spawnAerolite({(float)x, (float)y});
            // logMessage("clicked " + std::to_string(x) + ", " + std::to_string(y));
          } else if (SDL_GetMouseState(&x, &y) & SDL_BUTTON(SDL_BUTTON_RIGHT)) {
            SDL_SetRelativeMouseMode(SDL_FALSE);
          }
        }
        break;
      }
      case SDL_MOUSEMOTION:
        if (SDL_GetRelativeMouseMode())
          test_->updateMouse((float)sdl_event_.motion.xrel, (float)sdl_event_.motion.yrel);
        break;
      case SDL_MOUSEWHEEL:
        if (SDL_GetRelativeMouseMode())
          test_->updateZoom(((float)sdl_event_.wheel.y));
        break;
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
      backend_draw_ = !backend_draw_;
      break;
    case SDLK_F2:
      polygon_draw_ = !polygon_draw_;
      updatePolygonDraw();
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
  // Box2D
  Game::b2_world_.Step(delta_time, game.velocity_iterations_, game.position_iterations_);
  // Entities
  for (auto i = 0u; i <= GameEntity::game_entities_.highestActiveIndex(); ++i) {
    if (GameEntity::game_entities_.active(i)) {
      if (GameEntity::game_entities_[i].canBeDeactivated()) {
        GameEntity::game_entities_[i].free(i);
      } else {
        GameEntity::game_entities_[i].update(delta_time);
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

  for (auto i = 0u; i <= GameEntity::game_entities_.highestActiveIndex(); ++i) {
    if (GameEntity::game_entities_[i].type() == EntityTypes::Background) {
      GameEntity::game_entities_[i].draw();
      break;
    }
  }

  game.gui_sys_.titleText()->draw();

  if (debug_draw_) {
    Game::b2_world_.DebugDraw();
    b2_debug_.Draw();
  }

  if (backend_draw_) game.backend_sys_.draw();

  SDL_GL_SwapWindow(game.main_window_.getWindow());
}

ktp::GameState* ktp::TitleState::enter(Game& game) {
  game.reset();
  GameEntity::createEntity(EntityTypes::Background);
  GameEntity::createEntity(EntityTypes::Player);
  GameEntity::createEntity(EntityTypes::AeroliteSpawner);
  demo_time_ = SDL2_Timer::SDL2Ticks();
  game.gui_sys_.resetScore();
  return this;
}

void ktp::TitleState::handleEvents(Game& game) {
  while (SDL_PollEvent(&sdl_event_)) {
    ImGui_ImplSDL2_ProcessEvent(&sdl_event_);
    switch (sdl_event_.type) {
      case SDL_QUIT:
        game.quit_ = true;
        break;
      case SDL_KEYDOWN:
        if (!ImGui::GetIO().WantCaptureKeyboard) handleSDL2KeyEvents(game, sdl_event_.key.keysym.sym);
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
    case SDLK_F1:
      backend_draw_ = !backend_draw_;
      break;
    case SDLK_F2:
      polygon_draw_ = !polygon_draw_;
      updatePolygonDraw();
      break;
    default:
      demo_time_ = SDL2_Timer::SDL2Ticks();
      game.state_ = goToState(game, GameState::playing_);
      break;
  }
}

void ktp::TitleState::update(Game& game, float delta_time) {
  // Background
  for (auto i = 0u; i <= GameEntity::game_entities_.highestActiveIndex(); ++i) {
    GameEntity::game_entities_[i].update(delta_time * kDefaultBackgroundDeltaInMenu_);
  }
  // enter Demo mode
  if (SDL2_Timer::SDL2Ticks() - demo_time_ > kWaitForDemo_) {
    game.state_ = goToState(game, GameState::demo_);
  }
}
