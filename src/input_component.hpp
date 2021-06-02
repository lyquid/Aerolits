#ifndef AEROLITS_SRC_INPUT_COMPONENT_HPP_
#define AEROLITS_SRC_INPUT_COMPONENT_HPP_

namespace ktp {

class Player;

class InputComponent {
 public:

  void update(Player& player, float delta_time);

 private:
  
  inline static constexpr float kMaxDelta_ {0.1f};
};

using Input = InputComponent;

} // namespace ktp

#endif // AEROLITS_SRC_INPUT_COMPONENT_HPP_