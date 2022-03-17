#pragma once

#include <map>
#include <vector>

namespace ktp {

enum class EntityType {
  Undefined,
  Aerolite,
  Background,
  Emitter,
  Explosion,
  Player,
  PlayerDemo,
  Projectile,
  count
};

using EntityId       = unsigned int;
using EntitiesByType = std::map<EntityType, std::size_t>;
using GameEntities   = std::map<EntityId, EntityType>;

class EntityManager {
 public:

  EntityManager(const EntityManager& other) = delete;
  EntityManager(EntityManager&& other) = delete;

  EntityManager& operator=(const EntityManager& other) = delete;
  EntityManager& operator=(EntityManager&& other) = delete;

  static auto entitiesCount() { return entities_count_; }
  static auto entityCount(EntityType type) { return entities_by_type_[type]; }
  static EntityId createEntity(EntityType type);
  static void destroyEntity(EntityId id);
  static void init();
  static auto maxEntities() { return max_entities_; }

 private:

  static void buildEntity(EntityType type, EntityId id);
  static void unbuildEntity(EntityId id);

  static EntityId       current_entity_id_;
  static GameEntities   entities_;
  static EntitiesByType entities_by_type_;
  static unsigned int   entities_count_;
  static unsigned int   max_entities_;
};

} // namespace ktp
