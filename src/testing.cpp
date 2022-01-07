#include "include/aerolite.hpp"
#include "include/box2d_utils.hpp"
#include "include/palette.hpp"
#include "include/random.hpp"
#include "include/resources.hpp"
#include "include/testing.hpp"
#include "sdl2_wrappers/sdl2_geometry.hpp"
#include "sdl2_wrappers/sdl2_log.hpp"
#include "sdl2_wrappers/sdl2_timer.hpp"

void ktp::Testing::draw() {
  shader_program_.use();
  vao_.bind();
  glDrawElements(GL_TRIANGLES, (GLsizei)indices_data_.size(), GL_UNSIGNED_INT, 0);
  // bool show_demo_window {true};
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplSDL2_NewFrame();
  ImGui::NewFrame();
  // ImGui::ShowDemoWindow(&show_demo_window);

  if (show_another_window_) {
    ImGui::Begin("Another Window", &show_another_window_);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
    ImGui::Text("Hello from another window!");
    if (ImGui::Button("Close Me")) show_another_window_ = false;
    ImGui::End();
  }

  ImGui::Text("Hello, world %d", 123);
  // if (ImGui::Button("Save")) MySaveFunction();
  std::string buf {"holamanola"};
  ImGui::InputText("string", &buf);
  //ImGui::SliderFloat("float", &f, 0.0f, 1.0f);

  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void ktp::Testing::init() {
  shader_program_ = Resources::getShader("testing");

  vertices_data_ = SDL2_GL::cube(0.5f);
  EBO::generateEBO(vertices_data_, indices_data_);

  vao_.bind();
  indices_.setup(indices_data_);
  vertices_.setup(vertices_data_);
  vao_.linkAttrib(vertices_, 0, 3, GL_FLOAT, 0, nullptr);

  vertices_color_data_.resize(vertices_data_.size());
  for (auto& i: vertices_color_data_) {
    i = generateRand(0.f, 1.f);
  }
  colors_.setup(vertices_color_data_);
  vao_.linkAttrib(colors_, 1, 3, GL_FLOAT, 0, nullptr);
}

void ktp::Testing::update(float delta_time) {
  updateCamera(delta_time);
  updateMVP(delta_time);
}

void ktp::Testing::updateCamera(float delta_time) {
  const auto state {SDL_GetKeyboardState(nullptr)};
  if (state[SDL_SCANCODE_W]) {
    camera_.move(CameraMovement::Forward, delta_time);
  }
  if (state[SDL_SCANCODE_S]) {
    camera_.move(CameraMovement::Backward, delta_time);
  }
  if (state[SDL_SCANCODE_A]) {
    camera_.move(CameraMovement::Left, delta_time);
  }
  if (state[SDL_SCANCODE_D]) {
    camera_.move(CameraMovement::Right, delta_time);
  }
}

void ktp::Testing::updateMVP(float delta_time) {
  glm::mat4 model {1.f};
  const glm::mat4 mvp {camera_.projectionMatrix() * camera_.viewMatrix() * model};
  shader_program_.setMat4f("mvp", glm::value_ptr(mvp));
}
