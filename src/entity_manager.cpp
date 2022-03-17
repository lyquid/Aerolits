#include "include/aerolite.hpp"
#include "include/entity_manager.hpp"
#include <algorithm> // std::partition

using namespace ktp;

EntityId       EntityManager::current_entity_id_ {0};
GameEntities   EntityManager::entities_ {};
EntitiesByType EntityManager::entities_by_type_ {};
unsigned int   EntityManager::entities_count_ {0u};
unsigned int   EntityManager::max_entities_ {2000u};

void EntityManager::buildEntity(EntityType type, EntityId id) {
  switch(type) {
    case EntityType::Aerolite:
      AeroliteManager::activate(id);
      break;
    case EntityType::Background:
      // do things!
      break;
    case EntityType::Emitter:
      // do things!
      break;
    case EntityType::Explosion:
      // do things!
      break;
    case EntityType::Player:
      // do things!
      break;
    case EntityType::PlayerDemo:
      // do things!
      break;
    case EntityType::Projectile:
      // do things!
      break;
    case EntityType::Undefined:
      [[fallthrough]];
    default:
      // do things!
      break;
  }
}

EntityId EntityManager::createEntity(EntityType type) {
  if (entities_count_ < max_entities_) {
    // push id to entities/type map
    entities_.emplace(++current_entity_id_, type);
    // call the builder
    buildEntity(type, current_entity_id_);
    // increment the specific type/count map
    ++entities_by_type_[type];
    // increment total entities
    ++entities_count_;
    // return id
    return current_entity_id_;
  } else {
    // max entities reached!
    return 0;
  }
}

void EntityManager::destroyEntity(EntityId id) {
  const auto found {entities_.find(id)};
  if (found != entities_.end()) {
    // call the unbuilder
    unbuildEntity(id);
    // one less entity of type X
    --entities_by_type_[entities_[id]];
    // delete entity from entities map
    entities_.erase(id);
    // one less entity in global
    --entities_count_;
  } else {
    // entity id not found :(
  }
}

void EntityManager::init() {
  AeroliteManager::init();
}

void EntityManager::unbuildEntity(EntityId id) {
  const auto type {entities_[id]};
  switch(type) {
    case EntityType::Aerolite:
      AeroliteManager::deactivate(id);
      break;
    case EntityType::Background:
      // do things!
      break;
    case EntityType::Emitter:
      // do things!
      break;
    case EntityType::Explosion:
      // do things!
      break;
    case EntityType::Player:
      // do things!
      break;
    case EntityType::PlayerDemo:
      // do things!
      break;
    case EntityType::Projectile:
      // do things!
      break;
    case EntityType::Undefined:
      [[fallthrough]];
    default:
      // do things!
      break;
  }
}
