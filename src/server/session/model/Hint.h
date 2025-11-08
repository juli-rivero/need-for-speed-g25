#pragma once
#include "../physics/IPhysicalBody.h"
#include "server/session/model/Entity.h"

class Hint : public Entity {
   private:
    Vec2 position;
    float angle;

   public:
    Hint(int id, float x, float y, float angle)
        : Entity(id, EntityType::Hint), position({x, y}), angle(angle) {}

    int getId() const { return id; }
    Vec2 getPosition() const { return position; }
    float getAngle() const { return angle; }
};
