#pragma once

#include "graphics_component.hpp"
#include "opengl.hpp"
#include "physics_component.hpp"
#include "resources.hpp"
#include <array>
#include <utility> // std::move

namespace ktp {

struct Star {
  glm::vec4 color_ {};
  glm::vec2 delta_ {};
  glm::vec3 position_ {};
};

class BackgroundGraphicsComponent: public GraphicsComponent {

  friend class BackgroundPhysicsComponent;

 public:

  BackgroundGraphicsComponent();

  virtual void update(const GameEntity& background) override;

 private:

  glm::vec4 background_color_ {Palette::colorToGlmVec4(Palette::black)};
  const std::array<Color, 4> star_colors_ {Palette::purple,    Palette::copper_green,
                                           Palette::turquoise, Palette::yellow};
  VAO           vao_ {};
  VBO           vertices_ {};
  GLfloatVector vertices_data_ {};
  EBO           indices_ {};
  GLuintVector  indices_data_ {0, 1, 2, 0, 3, 2};
  ShaderProgram shader_ {Resources::getShader("star")};
  VBO           subdata_ {};
  glm::mat4     mvp_ {};
  GLuint        stars_count_ {};
};

class BackgroundPhysicsComponent: public PhysicsComponent {

 public:

  BackgroundPhysicsComponent(GameEntity* owner, BackgroundGraphicsComponent* graphics);
  BackgroundPhysicsComponent(const BackgroundPhysicsComponent& other) = delete;
  BackgroundPhysicsComponent(BackgroundPhysicsComponent&& other) { *this = std::move(other); }

  BackgroundPhysicsComponent& operator=(const BackgroundPhysicsComponent& other) = delete;
  BackgroundPhysicsComponent& operator=(BackgroundPhysicsComponent&& other);

  virtual void collide(const GameEntity* other) override {}
  virtual void update(const GameEntity& background, float delta_time) override;

 private:

  void generateStars();
  void updateMVP();
  static constexpr auto kComponents_ {6u};
  BackgroundGraphicsComponent* graphics_ {nullptr};
  std::vector<Star>            stars_ {};
  GLfloatVector                subdata_ {};
};

} // end namespace ktp
