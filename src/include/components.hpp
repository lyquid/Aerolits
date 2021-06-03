#ifndef AEROLITS_SRC_INCLUDE_COMPONENTS_HPP_
#define AEROLITS_SRC_INCLUDE_COMPONENTS_HPP_

namespace ktp {

class Player; // this will change to gameobject or something

class InputComponent {
 public:
  virtual ~InputComponent() {}
  virtual void update(Player& player, float delta_time) = 0;
};

class RendererComponent {
 public:
  virtual ~RendererComponent() {}
  virtual void update(Player& player) = 0;
};

} // namespace ktp

#endif // AEROLITS_SRC_INCLUDE_COMPONENTS_HPP_