#include "include/game.hpp"
#include "include/palette.hpp"
#include "include/particle.hpp"
#include "include/resources.hpp"

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
glm::mat4  ktp::PhysicsComponent::projection_ {};
b2World*   ktp::PhysicsComponent::world_ {nullptr};

/* GAME */

ktp::SDL2_Timer ktp::Game::gameplay_timer_ {};

ktp::Game::Game() {
  event_bus_.setSystems(&audio_sys_, &input_sys_, &gui_sys_, &output_sys_);
  GameEntity::event_bus_ = &event_bus_;
  SDL_LogSetAllPriority(SDL_LOG_PRIORITY_VERBOSE);
  if (!initSDL2()) return;
  logMessage("Box2D version: " + std::to_string(b2_version.major) + '.' + std::to_string(b2_version.minor) + '.' + std::to_string(b2_version.revision));
  if (!main_window_.create(kuge::GUISystem::kTitleText_ , screen_size_, SDL_WINDOW_OPENGL)) return;

  SDL2_GL::initGLEW(context_, main_window_);

  if (!loadResources()) return;
  // if (!gui_sys_.init(renderer_)) return false;

  // world_.SetDebugDraw(&g_debugDraw);
  // g_debugDraw.SetFlags(b2Draw::e_shapeBit | b2Draw::e_aabbBit | b2Draw::e_pairBit | b2Draw::e_centerOfMassBit);
  world_.SetContactListener(&contact_listener_);

  PhysicsComponent::setScreenSize({(float)screen_size_.x, (float)screen_size_.y});
  PhysicsComponent::setProjection(glm::ortho(
    0.f, (float)ConfigParser::game_config.screen_size_.x, // left, right
    0.f, (float)ConfigParser::game_config.screen_size_.y, // bottom, top
    -1.f, 1.f) // zNear, zFar
  );
  PhysicsComponent::setWorld(&world_);

  //state_ = GameState::goToState(*this, GameState::title_);
  state_ = GameState::goToState(*this, GameState::testing_);
}

void ktp::Game::clean() {
  Resources::cleanOpenGL();
  GameEntity::clear();
  clearB2World(world_);
  SDL2_Audio::closeMixer();
	SDL_Quit();
}

bool ktp::Game::initSDL2() {
  if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
    logSDL2Error("SDL_Init");
    return false;
  }
  if (!SDL2_Font::initTTF()) return false;
  if (!SDL2_Audio::initMixer()) return false;

  return true;
}

bool ktp::Game::loadResources() {
  if (!audio_sys_.loadResources()) {
    return false;
  }
  // fonts
  auto font_path {Resources::getResourcesPath("fonts") + "Future n0t Found.ttf"};
  Resources::loadFont("future", font_path, 512);

  // textures
  auto texture_path {Resources::getResourcesPath("textures") + "aerolite_00.png"};
  Resources::loadTexture("aerolite_00", texture_path, false);
  texture_path = Resources::getResourcesPath("textures") + "aerolite_01.png";
  Resources::loadTexture("aerolite_01", texture_path, false);
  Resources::loadTextureFromTextBlended("title", "Aeròlits", "future", Colors::white);

  // shaders
  auto vertex_shader_path {Resources::getResourcesPath("shaders") + "aerolite.vert"};
  auto fragment_shader_path {Resources::getResourcesPath("shaders") + "aerolite.frag"};
  Resources::loadShader("aerolite", vertex_shader_path, fragment_shader_path);
  vertex_shader_path = Resources::getResourcesPath("shaders") + "player.vert";
  fragment_shader_path = Resources::getResourcesPath("shaders") + "player.frag";
  Resources::loadShader("player", vertex_shader_path, fragment_shader_path);
  vertex_shader_path = Resources::getResourcesPath("shaders") + "projectile.vert";
  fragment_shader_path = Resources::getResourcesPath("shaders") + "projectile.frag";
  Resources::loadShader("projectile", vertex_shader_path, fragment_shader_path);
  vertex_shader_path = Resources::getResourcesPath("shaders") + "testing.vert";
  fragment_shader_path = Resources::getResourcesPath("shaders") + "testing.frag";
  Resources::loadShader("testing", vertex_shader_path, fragment_shader_path);

  return true;
}

void ktp::Game::reset() {
  GameEntity::clear();
  // we need to do this to prevent some of the bodies not being destroyed
  // ie: explosion particles if you pause and go to title
  clearB2World(world_);
}
