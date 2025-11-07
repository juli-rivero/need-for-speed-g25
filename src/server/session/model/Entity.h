
#ifndef TALLER_TP_ENTITY_H
#define TALLER_TP_ENTITY_H

enum class EntityType { Car, Wall, Checkpoint, Hint};


class Entity {
public:
    virtual ~Entity() = default;
    virtual EntityType getEntityType() const = 0;

};


#endif //TALLER_TP_ENTITY_H