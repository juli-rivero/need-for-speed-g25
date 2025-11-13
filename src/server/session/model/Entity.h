#pragma once

enum class EntityType { Car, Wall, Checkpoint, Hint, Bridge };

class Entity {
   protected:
    int id;
    EntityType type;

   public:
    Entity(int id, EntityType type) : id(id), type(type) {}
    virtual ~Entity() = default;
    EntityType getEntityType() const { return type; }
    virtual int getId() const { return id; }
};
