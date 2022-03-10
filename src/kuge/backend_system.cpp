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
    if (ImGui::BeginMenu("Draw options")) {

      ImGui::Checkbox("Box2D debug draw", &ktp::GameState::debug_draw_);

      if (ImGui::Checkbox("OpenGL culling", &ktp::GameState::culling_)) {
        ktp::GameState::updateCulling();
      }

      if (ImGui::Checkbox("OpenGL deep test", &ktp::GameState::deep_test_)) {
        ktp::GameState::updateDeepTest();
      }

      if (ImGui::Checkbox("OpenGL polygon mode", &ktp::GameState::polygon_draw_)) {
        ktp::GameState::updatePolygonDraw();
      }

      ImGui::EndMenu();
    }
    ImGui::EndMainMenuBar();
  }
}
