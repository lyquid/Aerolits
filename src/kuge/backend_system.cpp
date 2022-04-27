#include "backend_system.hpp"
#include "../include/game.hpp"
#include "../include/game_entity.hpp"
#include "../include/game_state.hpp"
#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl.h"

void kuge::BackendSystem::draw() {
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplSDL2_NewFrame();
  ImGui::NewFrame();

  mainMenuBar();
  if (statistics_) statisticsWindow(&statistics_);

  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void kuge::BackendSystem::mainMenuBar() {
  if (ImGui::BeginMainMenuBar()) {

    if (ImGui::BeginMenu("Statistics")) {
      ImGui::Checkbox("Show statistics", &statistics_);
      ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("OpenGL options")) {
      if (ImGui::Checkbox("Culling", &ktp::GameState::culling_)) {
        ktp::GameState::updateCulling();
      }
      if (ImGui::Checkbox("Deep test", &ktp::GameState::deep_test_)) {
        ktp::GameState::updateDeepTest();
      }
      if (ImGui::Checkbox("Polygon mode", &ktp::GameState::polygon_draw_)) {
        ktp::GameState::updatePolygonDraw();
      }
      ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Box2D debug")) {
      ImGui::Checkbox("Debug draw", &ktp::GameState::debug_draw_);

      ImGui::BeginDisabled(!ktp::GameState::debug_draw_);
        if (ImGui::Checkbox("AABBs", &debug_flags_.aabb)) {
          ktp::GameState::setDebugDrawFlags(debug_flags_);
        }
        if (ImGui::Checkbox("Broad-phase pairs", &debug_flags_.pairs)) {
          ktp::GameState::setDebugDrawFlags(debug_flags_);
        }
        if (ImGui::Checkbox("Center of mass", &debug_flags_.center)) {
          ktp::GameState::setDebugDrawFlags(debug_flags_);
        }
        if (ImGui::Checkbox("Joint connections", &debug_flags_.joints)) {
          ktp::GameState::setDebugDrawFlags(debug_flags_);
        }
        if (ImGui::Checkbox("Shapes", &debug_flags_.shapes)) {
          ktp::GameState::setDebugDrawFlags(debug_flags_);
        }
      ImGui::EndDisabled();

      ImGui::EndMenu();
    }

    ImGui::EndMainMenuBar();
  }
}

void kuge::BackendSystem::statisticsWindow(bool* show) {
  static int corner {1};
  ImGuiIO& io {ImGui::GetIO()};
  ImGuiWindowFlags window_flags {ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav};
  // position
  if (corner != -1) {
    constexpr auto kPad {10.f};
    const ImGuiViewport* viewport {ImGui::GetMainViewport()};
    ImVec2 work_pos {viewport->WorkPos}; // Use work area to avoid menu-bar/task-bar, if any!
    ImVec2 work_size {viewport->WorkSize};
    ImVec2 window_pos {}, window_pos_pivot {};
    window_pos.x = (corner & 1) ? (work_pos.x + work_size.x - kPad) : (work_pos.x + kPad);
    window_pos.y = (corner & 2) ? (work_pos.y + work_size.y - kPad) : (work_pos.y + kPad);
    window_pos_pivot.x = (corner & 1) ? 1.f : 0.f;
    window_pos_pivot.y = (corner & 2) ? 1.f : 0.f;
    ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
    window_flags |= ImGuiWindowFlags_NoMove;
  }

  ImGui::SetNextWindowBgAlpha(0.3f);

  if (ImGui::Begin("Statistics", show, window_flags)) {
    ImGui::Text("Frame time: %.3fs.", ktp::Game::frame_time_);
    if (ImGui::IsMousePosValid())
      ImGui::Text("Mouse Position: (%.1f,%.1f)", io.MousePos.x, io.MousePos.y);
    else
      ImGui::Text("Mouse Position: <invalid>");

    ImGui::Separator();
    ImGui::Text("B2Bodies: %i", ktp::Game::b2_world_.GetBodyCount());

    ImGui::Separator();
    ImGui::Text("Entities\t%i/%i", ktp::GameEntity::count(), ktp::GameEntity::game_entities_.capacity());
    ImGui::Text("Player:     %i",  ktp::GameEntity::entitiesCount(ktp::EntityTypes::Player) + ktp::GameEntity::entitiesCount(ktp::EntityTypes::PlayerDemo));
    ImGui::Text("Background: %i",  ktp::GameEntity::entitiesCount(ktp::EntityTypes::Background));
    ImGui::Text("Aerolite:   %i",  ktp::GameEntity::entitiesCount(ktp::EntityTypes::Aerolite));
    ImGui::Text("Projectile: %i",  ktp::GameEntity::entitiesCount(ktp::EntityTypes::Projectile));
    ImGui::Text("Emitter:    %i",  ktp::GameEntity::entitiesCount(ktp::EntityTypes::Emitter));
    ImGui::Text("Explosion:  %i",  ktp::GameEntity::entitiesCount(ktp::EntityTypes::Explosion));

    if (ImGui::BeginPopupContextWindow()) {
      if (ImGui::MenuItem("Custom",       nullptr, corner == -1)) corner = -1;
      if (ImGui::MenuItem("Top-left",     nullptr, corner ==  0)) corner =  0;
      if (ImGui::MenuItem("Top-right",    nullptr, corner ==  1)) corner =  1;
      if (ImGui::MenuItem("Bottom-left",  nullptr, corner ==  2)) corner =  2;
      if (ImGui::MenuItem("Bottom-right", nullptr, corner ==  3)) corner =  3;
      if (show && ImGui::MenuItem("Close")) *show = false;
      ImGui::EndPopup();
    }
  }
  ImGui::End();
}
