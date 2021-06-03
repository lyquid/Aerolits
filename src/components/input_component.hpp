#ifndef AEROLITS_SRC_COMPONENTS_INPUT_COMPONENT_HPP_
#define AEROLITS_SRC_COMPONENTS_INPUT_COMPONENT_HPP_

namespace ktp {

class Player; // this will change to gameobject or something

class InputComponent {
 public:
  virtual ~InputComponent() {}
  virtual void update(Player& player, float delta_time) = 0;
};

} // namespace ktp

#endif // AEROLITS_SRC_COMPONENTS_INPUT_COMPONENT_HPP_