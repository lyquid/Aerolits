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
  } else if (entity_a->type() == EntityTypes::Player || entity_a->type() == EntityTypes::PlayerDemo) {
    playerCollision(entity_a, entity_b, contact);
  } else if (entity_b->type() == EntityTypes::Player || entity_b->type() == EntityTypes::PlayerDemo) {
    playerCollision(entity_b, entity_a, contact);
  } else if (entity_a->type() == EntityTypes::Projectile) {
    projectileCollision(entity_a, entity_b);
  } else if (entity_b->type() == EntityTypes::Projectile) {
    projectileCollision(entity_b, entity_a);
  }
}

void ktp::ContactListener::PostSolve(b2Contact* contact, const b2ContactImpulse* impulse) {
  /* if (!contact->GetFixtureA()->GetBody()->GetUserData().pointer
   || !contact->GetFixtureB()->GetBody()->GetUserData().pointer) return; */

  // const auto entity_a {reinterpret_cast<GameEntity*>(contact->GetFixtureA()->GetBody()->GetUserData().pointer)};
  // const auto entity_b {reinterpret_cast<GameEntity*>(contact->GetFixtureB()->GetBody()->GetUserData().pointer)};

  // if (entity_a->type() == EntityTypes::Player || entity_a->type() == EntityTypes::PlayerDemo) {
  //   // if (impulse->normalImpulses[0] > 1.f || impulse->normalImpulses[1] > 1.f)
  //     // logMessage("Player A, [PLAYER] = " + std::to_string(impulse->normalImpulses[0]) + ", [AEROLITE] = " + std::to_string(impulse->normalImpulses[1]) + " TOTAL = " + std::to_string(impulse->normalImpulses[0] + impulse->normalImpulses[1]));
  //   // playerCollision(entity_a, entity_b, impulse->normalImpulses[0]);
  // } else if (entity_b->type() == EntityTypes::Player || entity_b->type() == EntityTypes::PlayerDemo) {
  //   // if (impulse->normalImpulses[0] > 1.f || impulse->normalImpulses[1] > 1.f)
  //     // logMessage("Player B, [AEROLITE] = " + std::to_string(impulse->normalImpulses[0]) + ", [PLAYER] = " + std::to_string(impulse->normalImpulses[1]) + " TOTAL = " + std::to_string(impulse->normalImpulses[0] + impulse->normalImpulses[1]));
  //   // playerCollision(entity_b, entity_a, impulse->normalImpulses[0]);
  // }
}

void ktp::ContactListener::PreSolve(b2Contact* contact, const b2Manifold* oldManifold) {
  const auto entity_a {reinterpret_cast<GameEntity*>(contact->GetFixtureA()->GetBody()->GetUserData().pointer)};
  const auto entity_b {reinterpret_cast<GameEntity*>(contact->GetFixtureB()->GetBody()->GetUserData().pointer)};
  PlayerPhysicsComponent* player_phy {nullptr};
  if (entity_a->type() == EntityTypes::Player || entity_a->type() == EntityTypes::PlayerDemo) {
    player_phy = static_cast<PlayerPhysicsComponent*>(entity_a->physics());
  } else if (entity_b->type() == EntityTypes::Player || entity_b->type() == EntityTypes::PlayerDemo) {
    player_phy = static_cast<PlayerPhysicsComponent*>(entity_b->physics());
  } else {
    return;
  }
  b2WorldManifold world_manifold {};
  contact->GetWorldManifold(&world_manifold);
  const auto point {world_manifold.points[0]};
  const auto body_a_v {contact->GetFixtureA()->GetBody()->GetLinearVelocityFromWorldPoint(point)};
  const auto body_b_v {contact->GetFixtureB()->GetBody()->GetLinearVelocityFromWorldPoint(point)};
  player_phy->setApproachVelocity(b2Dot(body_b_v - body_a_v, world_manifold.normal));
}

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
      // playerCollision(other, aerolite, contact);
      break;
    case EntityTypes::PlayerDemo:
      // playerCollision(other, aerolite, contact);
      break;
    case EntityTypes::Projectile:
      other->physics()->collide(aerolite);
      break;
  }
}

void ktp::ContactListener::explosionCollision(GameEntity* explosion, GameEntity* other) {
  switch (other->type()) {
    case EntityTypes::AeroliteSpawner:
      other->physics()->collide(explosion);
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

void ktp::ContactListener::playerCollision(GameEntity* player, GameEntity* other, b2Contact* contact) {
  // switch (other->type()) {
  //   case EntityTypes::Aerolite:
  //     const auto player_phy {static_cast<PlayerPhysicsComponent*>(player->physics())};
  //     const auto player_linear_velocity {player_phy->body()->GetLinearVelocityFromWorldPoint(player_phy->worldManifold()->points[0])};
  //     // other->physics()->collide(player);

  //     const auto aerolite_phy {static_cast<AerolitePhysicsComponent*>(other->physics())};
  //     const auto aerolite_linear_velocity {aerolite_phy->body()->GetLinearVelocityFromWorldPoint(aerolite_phy->worldManifold()->points[0])};

  //     const auto impact_velocity {player_linear_velocity - aerolite_linear_velocity};
  //     logMessage("impact vel -> x = " + std::to_string(impact_velocity.x) + ", y = " + std::to_string(impact_velocity.y)
  //       + " sum = " + std::to_string(std::abs(impact_velocity.x) + std::abs(impact_velocity.y)));
  //     break;
  // }
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
