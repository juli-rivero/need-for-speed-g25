
#ifndef TALLER_TP_ENTITY_H
#define TALLER_TP_ENTITY_H

enum class EntityType { Car, Wall, Checkpoint, Hint};


class Entity {
protected:
    int id;
    EntityType type;
public:
    Entity(int id, EntityType type) : id(id), type(type) {}
    virtual ~Entity() = default;
    EntityType getEntityType() const { return type; }

};


#endif //TALLER_TP_ENTITY_H