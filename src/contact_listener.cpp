#include "include/aerolite.hpp"
#include "include/contact_listener.hpp"
#include "include/game_entity.hpp"

void ktp::ContactListener::BeginContact(b2Contact* contact) {
    if (!contact->GetFixtureA()->GetBody()->GetUserData().pointer
     || !contact->GetFixtureB()->GetBody()->GetUserData().pointer) return;

    const auto fixture_A {reinterpret_cast<GameEntity*>(contact->GetFixtureA()->GetBody()->GetUserData().pointer)};
    const auto fixture_B {reinterpret_cast<GameEntity*>(contact->GetFixtureB()->GetBody()->GetUserData().pointer)};

    switch (fixture_A->type()) {
      case EntityTypes::Aerolite:
        if (fixture_B->type() == EntityTypes::Player || fixture_B->type() == EntityTypes::PlayerDemo) {
          // kill player
        } else if (fixture_B->type() == EntityTypes::ExplosionParticle) {
          contact->GetWorldManifold(static_cast<AerolitePhysicsComponent*>(fixture_A->physics())->worldManifold());
          fixture_A->physics()->collide(fixture_B);
          fixture_B->physics()->collide(fixture_A);
        } else if (fixture_B->type() == EntityTypes::Projectile) {
          fixture_B->physics()->collide(fixture_A);
        }
        break;
      case EntityTypes::Background:
        break;
      case EntityTypes::ExplosionParticle:
        if (fixture_B->type() == EntityTypes::Aerolite) {
          contact->GetWorldManifold(static_cast<AerolitePhysicsComponent*>(fixture_B->physics())->worldManifold());
          fixture_A->physics()->collide(fixture_B);
          fixture_B->physics()->collide(fixture_A);
        } else if (fixture_B->type() == EntityTypes::Projectile) {
          fixture_A->physics()->collide(fixture_B);
          fixture_B->physics()->collide(fixture_A);
        }
        break;
      case EntityTypes::Player:
        break;
      case EntityTypes::PlayerDemo:
        break;
      case EntityTypes::Projectile:
        if (fixture_B->type() == EntityTypes::Aerolite) {
          fixture_A->physics()->collide(fixture_B);
        } else if (fixture_B->type() == EntityTypes::Player || fixture_B->type() == EntityTypes::PlayerDemo) {
          // kill player or something
        } else if (fixture_B->type() == EntityTypes::ExplosionParticle) {
          fixture_A->physics()->collide(fixture_B);
          fixture_B->physics()->collide(fixture_A);
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
