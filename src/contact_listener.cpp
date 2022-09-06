#include "include/aerolite.hpp"
#include "include/contact_listener.hpp"
#include "include/game_entity.hpp"

void ktp::ContactListener::BeginContact(b2Contact* contact) {
  if (!contact->GetFixtureA()->GetBody()->GetUserData().pointer
   || !contact->GetFixtureB()->GetBody()->GetUserData().pointer) return;

  const auto entity_a {reinterpret_cast<GameEntity*>(contact->GetFixtureA()->GetBody()->GetUserData().pointer)};
  const auto entity_b {reinterpret_cast<GameEntity*>(contact->GetFixtureB()->GetBody()->GetUserData().pointer)};

  if (entity_a->type() == EntityTypes::Aerolite) {
    aeroliteCollision(entity_a, entity_b, contact);
  } else if (entity_b->type() == EntityTypes::Aerolite) {
    aeroliteCollision(entity_b, entity_a, contact);
  } else if (entity_a->type() == EntityTypes::Explosion) {
    explosionCollision(entity_a, entity_b);
  } else if (entity_b->type() == EntityTypes::Explosion) {
    explosionCollision(entity_b, entity_a);
  } else if (entity_a->type() == EntityTypes::Projectile) {
    projectileCollision(entity_a, entity_b);
  } else if (entity_b->type() == EntityTypes::Projectile) {
    projectileCollision(entity_b, entity_a);
  }
}

void ktp::ContactListener::EndContact(b2Contact* contact) {}

void ktp::ContactListener::aeroliteCollision(GameEntity* aerolite, GameEntity* other, b2Contact* contact) {
  switch(other->type()) {
    case EntityTypes::Aerolite:
      // play sound?
      break;
    case EntityTypes::AeroliteSpawner:
      other->physics()->collide(aerolite);
      break;
    case EntityTypes::Explosion:
      contact->GetWorldManifold(static_cast<AerolitePhysicsComponent*>(aerolite->physics())->worldManifold());
      aerolite->physics()->collide(other);
      other->physics()->collide(aerolite);
      break;
    case EntityTypes::Player:
      // kill player?
      break;
    case EntityTypes::PlayerDemo:
      // kill!!
      break;
    case EntityTypes::Projectile:
      other->physics()->collide(aerolite);
      break;
  }
}

void ktp::ContactListener::explosionCollision(GameEntity* explosion, GameEntity* other) {
  switch (other->type()) {
    case EntityTypes::AeroliteSpawner:
      // do something
      break;
    case EntityTypes::Projectile:
      explosion->physics()->collide(other);
      other->physics()->collide(explosion);
      break;
    case EntityTypes::Player:
      // bum!
      break;
    case EntityTypes::PlayerDemo:
      // kabum!
      break;
  }
}

void ktp::ContactListener::projectileCollision(GameEntity* projectile, GameEntity* other) {
  switch (other->type()) {
    case EntityTypes::AeroliteSpawner:
      other->physics()->collide(projectile);
      break;
    case EntityTypes::Player:
      // bum!
      break;
    case EntityTypes::PlayerDemo:
      // kabum!
      break;
  }
}
