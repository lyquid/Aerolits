#include "backend_system.hpp"
#include "../include/game_state.hpp"
#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl.h"

void kuge::BackendSystem::draw() {
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplSDL2_NewFrame();
  ImGui::NewFrame();

  mainMenuBar();

  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void kuge::BackendSystem::mainMenuBar() {
  if (ImGui::BeginMainMenuBar()) {
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
