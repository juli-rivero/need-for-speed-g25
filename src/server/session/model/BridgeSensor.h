#pragma once
#include <memory>

#include "Entity.h"
#include "common/structs.h"
#include "server/session/physics/IPhysicalBody.h"

class BridgeSensor : public Entity {
   private:
    BridgeSensorType sensorType;
    std::shared_ptr<IPhysicalBody> body;
    float width;
    float height;

   public:
    BridgeSensor(int id, BridgeSensorType type,
                 std::shared_ptr<IPhysicalBody> body, float w, float h)
        : Entity(id, EntityType::BridgeSensor),
          sensorType(type),
          body(body),
          width(w),
          height(h) {}

    BridgeSensorType getType() const { return sensorType; }
    Vec2 getPosition() const { return body->getPosition(); }
    float getWidth() const { return width; }
    float getHeight() const { return height; }
    static constexpr EntityType Type = EntityType::BridgeSensor;
};
