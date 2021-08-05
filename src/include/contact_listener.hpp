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
    if (!contact->GetFixtureA()->GetBody()->GetUserData().pointer
     || !contact->GetFixtureB()->GetBody()->GetUserData().pointer) return;

    const auto fixture_A = reinterpret_cast<GameEntity*>(contact->GetFixtureA()->GetBody()->GetUserData().pointer);
    const auto fixture_B = reinterpret_cast<GameEntity*>(contact->GetFixtureB()->GetBody()->GetUserData().pointer);

    switch (fixture_A->type()) {
      case EntityTypes::Aerolite:
        if (fixture_B->type() == EntityTypes::Player || fixture_B->type() == EntityTypes::PlayerDemo) {
          // fixture_B->toBeDeactivated();
        } else if (fixture_B->type() == EntityTypes::Projectile) {
          fixture_A->physics()->collide(fixture_B);
          fixture_B->physics()->collide(fixture_A);
        }
        break;
      case EntityTypes::Background:
        break;
      case EntityTypes::Player:
        if (fixture_B->type() == EntityTypes::Aerolite) {
          // fixture_A->toBeDeactivated();
        } else if (fixture_B->type() == EntityTypes::Projectile) {
          // fixture_A->toBeDeactivated();
          // fixture_B->toBeDeactivated();
        }
        break;
      case EntityTypes::PlayerDemo:
        if (fixture_B->type() == EntityTypes::Aerolite) {
          // fixture_A->toBeDeactivated();
        } else if (fixture_B->type() == EntityTypes::Projectile) {
          // fixture_A->toBeDeactivated();
          // fixture_B->toBeDeactivated();
        }
        break;
      case EntityTypes::Projectile:
        if (fixture_B->type() == EntityTypes::Aerolite) {
          fixture_A->physics()->collide(fixture_B);
          fixture_B->physics()->collide(fixture_A);
        } else if (fixture_B->type() == EntityTypes::Player || fixture_B->type() == EntityTypes::PlayerDemo) {
          // fixture_A->toBeDeactivated();
          // fixture_B->toBeDeactivated();
        } else if (fixture_B->type() == EntityTypes::Projectile) {
          fixture_A->physics()->collide(fixture_B);
          fixture_B->physics()->collide(fixture_A);
        }
        break;
      case EntityTypes::Undefined:
      case EntityTypes::count:
      default:
        return;
    }
  }

  /**
   * @brief Called when two fixtures cease to touch.
   *
   * @param contact
   */
  virtual void EndContact(b2Contact* contact) override {}

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