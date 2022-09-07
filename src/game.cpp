#include "include/debug_draw.hpp"
#include "include/game.hpp"
#include "include/game_entity.hpp"
#include "include/palette.hpp"
#include "include/particle.hpp"
#include "include/resources.hpp"
#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"

/* kuge/system.hpp */
const kuge::EventBus* kuge::System::event_bus_ {event_bus_};

/* kuge/event.cpp */
const ktp::SDL2_Timer& kuge::KugeEvent::gameplay_timer_ {ktp::Game::gameplay_timer_};

/* include/game_entity.hpp */
kuge::EventBus*    ktp::GameEntity::event_bus_ {nullptr};
ktp::EntitiesCount ktp::GameEntity::entities_count_ {};
ktp::EntitiesPool  ktp::GameEntity::game_entities_ {1000};

/* include/physics_component.hpp */
SDL_FPoint   ktp::PhysicsComponent::b2_screen_size_ {};
ktp::Camera& ktp::PhysicsComponent::camera_ {Game::camera_};
b2World*     ktp::PhysicsComponent::world_ {nullptr};

/* GAME */

ktp::Camera ktp::Game::camera_ {};

double ktp::Game::frame_time_ {};

ktp::SDL2_Timer ktp::Game::gameplay_timer_ {};

b2World ktp::Game::b2_world_ {b2Vec2{0.f, 0.f}};

ktp::Game::Game() {
  event_bus_.setSystems(&audio_sys_, &backend_sys_, &input_sys_, &gui_sys_, &output_sys_);
  GameEntity::event_bus_ = &event_bus_;
  SDL_LogSetAllPriority(SDL_LOG_PRIORITY_VERBOSE);
  if (!initSDL2()) return;
  logMessage("Box2D version: " + std::to_string(b2_version.major) + '.' + std::to_string(b2_version.minor) + '.' + std::to_string(b2_version.revision));
  if (!main_window_.create(kuge::GUISystem::kTitleText_ , screen_size_, SDL_WINDOW_OPENGL)) return;

  SDL2_GL::initGLEW(context_, main_window_);

  initImgui();

  if (!loadResources()) return;
  gui_sys_.init();

  b2_world_.SetDebugDraw(&GameState::b2_debug_);
  GameState::b2_debug_.Init();
  b2_world_.SetContactListener(&contact_listener_);

  PhysicsComponent::setScreenSize({(float)screen_size_.x, (float)screen_size_.y});
  PhysicsComponent::setWorld(&b2_world_);

  camera_.setOrthographicMatrix(glm::ortho(
    0.f, (float)screen_size_.x, // left, right
    0.f, (float)screen_size_.y, // bottom, top
    -1.f, 1.f) // zNear, zFar
  );
  camera_.setProjection(Projection::Orthographic);

  state_ = GameState::goToState(*this, GameState::title_);
  // state_ = GameState::goToState(*this, GameState::testing_);
  backend_sys_.resetStatistics();
}

void ktp::Game::clean() {
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplSDL2_Shutdown();
  ImGui::DestroyContext();
  Resources::cleanOpenGL();
  GameEntity::clear();
  clearB2World(b2_world_);
  SDL2_Audio::closeMixer();
	SDL_Quit();
}

bool ktp::Game::initImgui() {
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io {ImGui::GetIO()};
  (void)io;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
  // io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
  // Setup Dear ImGui style
  ImGui::StyleColorsDark();
  // Setup Platform/Renderer backends
  ImGui_ImplSDL2_InitForOpenGL(main_window_.getWindow(), context_.context());
  ImGui_ImplOpenGL3_Init("#version 330");
  return true;
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
  texture_path = Resources::getResourcesPath("textures") + "particle_01.png";
  Resources::loadTexture("particle_01", texture_path, false);
  texture_path = Resources::getResourcesPath("textures") + "particle_02.png";
  Resources::loadTexture("particle_02", texture_path, true);
  // shaders
  auto vertex_shader_path {Resources::getResourcesPath("shaders") + "aerolite.vert"};
  auto fragment_shader_path {Resources::getResourcesPath("shaders") + "aerolite.frag"};
  if (!Resources::loadShader("aerolite", vertex_shader_path, fragment_shader_path)) return false;
  vertex_shader_path = Resources::getResourcesPath("shaders") + "aerolite_arrow.vert";
  fragment_shader_path = Resources::getResourcesPath("shaders") + "aerolite_arrow.frag";
  if (!Resources::loadShader("aerolite_arrow", vertex_shader_path, fragment_shader_path)) return false;
  vertex_shader_path = Resources::getResourcesPath("shaders") + "cube.vert";
  fragment_shader_path = Resources::getResourcesPath("shaders") + "cube.frag";
  if (!Resources::loadShader("cube", vertex_shader_path, fragment_shader_path)) return false;
  vertex_shader_path = Resources::getResourcesPath("shaders") + "debug_draw_lines.vert";
  fragment_shader_path = Resources::getResourcesPath("shaders") + "debug_draw.frag";
  if (!Resources::loadShader("debug_draw_lines", vertex_shader_path, fragment_shader_path)) return false;
  vertex_shader_path = Resources::getResourcesPath("shaders") + "debug_draw_points.vert";
  fragment_shader_path = Resources::getResourcesPath("shaders") + "debug_draw.frag";
  if (!Resources::loadShader("debug_draw_points", vertex_shader_path, fragment_shader_path)) return false;
  vertex_shader_path = Resources::getResourcesPath("shaders") + "debug_draw_triangles.vert";
  fragment_shader_path = Resources::getResourcesPath("shaders") + "debug_draw.frag";
  if (!Resources::loadShader("debug_draw_triangles", vertex_shader_path, fragment_shader_path)) return false;
  vertex_shader_path = Resources::getResourcesPath("shaders") + "explosion.vert";
  fragment_shader_path = Resources::getResourcesPath("shaders") + "explosion.frag";
  if (!Resources::loadShader("explosion", vertex_shader_path, fragment_shader_path)) return false;
  vertex_shader_path = Resources::getResourcesPath("shaders") + "gui_string.vert";
  fragment_shader_path = Resources::getResourcesPath("shaders") + "gui_string.frag";
  if (!Resources::loadShader("gui_string", vertex_shader_path, fragment_shader_path)) return false;
  vertex_shader_path = Resources::getResourcesPath("shaders") + "particle.vert";
  fragment_shader_path = Resources::getResourcesPath("shaders") + "particle.frag";
  if (!Resources::loadShader("particle", vertex_shader_path, fragment_shader_path)) return false;
  vertex_shader_path = Resources::getResourcesPath("shaders") + "player.vert";
  fragment_shader_path = Resources::getResourcesPath("shaders") + "player.frag";
  if (!Resources::loadShader("player", vertex_shader_path, fragment_shader_path)) return false;
  vertex_shader_path = Resources::getResourcesPath("shaders") + "projectile.vert";
  fragment_shader_path = Resources::getResourcesPath("shaders") + "projectile.frag";
  if (!Resources::loadShader("projectile", vertex_shader_path, fragment_shader_path)) return false;
  vertex_shader_path = Resources::getResourcesPath("shaders") + "star.vert";
  fragment_shader_path = Resources::getResourcesPath("shaders") + "star.frag";
  if (!Resources::loadShader("star", vertex_shader_path, fragment_shader_path)) return false;
  vertex_shader_path = Resources::getResourcesPath("shaders") + "test.vert";
  fragment_shader_path = Resources::getResourcesPath("shaders") + "test.frag";
  if (!Resources::loadShader("test", vertex_shader_path, fragment_shader_path)) return false;

  return true;
}

void ktp::Game::reset() {
  GameEntity::clear();
  // we need to do this to prevent some of the bodies not being destroyed
  // ie: explosion particles if you pause and go to title
  clearB2World(b2_world_);
}
