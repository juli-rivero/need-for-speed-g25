#pragma once

#include <cmath>
#include <memory>
#include <utility>

#include "../physics/IPhysicalBody.h"
#include "Entity.h"
#include "common/structs.h"

class Checkpoint : public Entity {
   private:
    int order;
    CheckpointType checkpointType;
    float width;
    float height;
    float angle;
    std::shared_ptr<IPhysicalBody> body;

   public:
    Checkpoint(int id, int order, CheckpointType checkpointType, float width,
               float height, float angle, std::shared_ptr<IPhysicalBody> body)
        : Entity(id, EntityType::Checkpoint),
          order(order),
          checkpointType(checkpointType),
          width(width),
          height(height),
          angle(angle),
          body(std::move(body)) {}

    int getOrder() const { return order; }

    Vec2 getPosition() const { return body->getPosition(); }
    float getAngle() const { return angle; }
    float getWidth() const { return width; }
    float getHeight() const { return height; }
    CheckpointType getType() const { return checkpointType; }
};
