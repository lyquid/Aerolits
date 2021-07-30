#pragma once

#include "../../sdl2_wrappers/sdl2_log.hpp"
#include "aerolite.hpp"
#include "game_entity.hpp"
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
    auto fixture_A = reinterpret_cast<GameEntity*>(contact->GetFixtureA()->GetBody()->GetUserData().pointer);
    auto fixture_B = reinterpret_cast<GameEntity*>(contact->GetFixtureB()->GetBody()->GetUserData().pointer);

    /* switch (fixture_A->type()) {
      case EntityTypes::Aerolite:
        if (fixture_B->type() == EntityTypes::Player || fixture_B->type() == EntityTypes::PlayerDemo) {
          // fixture_B->physics()->toBeDeleted();
        } else if (fixture_B->type() == EntityTypes::Projectile) {
          fixture_A->physics()->toBeDeleted();   // dont like this
          fixture_A->increaseAeroliteCount(-1);
          fixture_B->physics()->toBeDeleted();
        }
        break;
      case EntityTypes::Background:
        break;
      case EntityTypes::Player:
        if (fixture_B->type() == EntityTypes::Aerolite) {
          // fixture_A->physics()->toBeDeleted();
        }
        break;
      case EntityTypes::PlayerDemo:
        if (fixture_B->type() == EntityTypes::Aerolite) {
          // fixture_A->physics()->toBeDeleted();
        }
        break;
      case EntityTypes::Projectile:
        if (fixture_B->type() == EntityTypes::Aerolite) {
          fixture_A->physics()->toBeDeleted();
          fixture_B->physics()->toBeDeleted();
          fixture_B->increaseAeroliteCount(-1);
        } else if (fixture_B->type() == EntityTypes::Player || fixture_B->type() == EntityTypes::PlayerDemo) {
          //fixture_A->physics()->toBeDeleted();
          //fixture_B->physics()->toBeDeleted();
        } else if (fixture_B->type() == EntityTypes::Projectile) {
          fixture_A->physics()->toBeDeleted();
          fixture_B->physics()->toBeDeleted();
        }
        break;
      case EntityTypes::Undefined:
      case EntityTypes::count:
      default:
        return;
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

  std::string checkType(EntityTypes type) const {
    switch (type) {
      case EntityTypes::Aerolite:
        return "Aerolite";
        break;
      case EntityTypes::Background:
        return "Background";
        break;
      case EntityTypes::Player:
        return "Player";
        break;
      case EntityTypes::PlayerDemo:
        return "PlayerDemo";
        break;
      case EntityTypes::Projectile:
        return "Projectile";
        break;
    }
    return "nothing";
  }
};

} // namespace ktp