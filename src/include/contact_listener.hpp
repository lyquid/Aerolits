#pragma once

// #include "../../sdl2_wrappers/sdl2_log.hpp"
#include <box2d/box2d.h>

namespace ktp {

class ContactListener: public b2ContactListener {

 public:

  /**
   * @brief Called when two fixtures begin to touch.
   *
   * @param contact
   */
  virtual void BeginContact(b2Contact* contact) override {
    auto user_data {contact->GetFixtureA()->GetBody()->GetUserData()};
    //logMessage(typeid(user_data.pointer).name());

    // auto p2 = reinterpret_cast<GameEntity*>(user_data.pointer)->getType();
    /* switch(reinterpret_cast<GameEntity*>(user_data.pointer)->getType()) {
    //switch(dynamic_cast<GameEntity*>(user_data.pointer)->getType()) {
      case GameEntities::Aerolite:
        //logMessage("aerolite");
        break;
      default:
        //logMessage("default");
        break;
    } */
  }

  /**
   * @brief Called when two fixtures cease to touch.
   *
   * @param contact
   */
  virtual void EndContact(b2Contact* contact) override {
    //logMessage("end contact");
    auto user_data {contact->GetFixtureA()->GetBody()->GetUserData()};
  }

 private:

};

} // namespace ktp