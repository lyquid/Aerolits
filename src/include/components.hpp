#ifndef AEROLITS_SRC_INCLUDE_COMPONENTS_HPP_
#define AEROLITS_SRC_INCLUDE_COMPONENTS_HPP_

namespace ktp {

class GameObject;
class Player; // this will change to gameobject or something
class SDL2_Renderer;

class InputComponent {
 public:
  virtual ~InputComponent() {}
  virtual void update(Player&, float) = 0;
};

class RendererComponent {
 public:
  virtual ~RendererComponent() {}
  virtual void update(Player&, SDL2_Renderer&) = 0;
};

} // namespace ktp

#endif // AEROLITS_SRC_INCLUDE_COMPONENTS_HPP_