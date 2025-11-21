#pragma once
#include <memory>

#include "Entity.h"
#include "server/session/physics/IPhysicalBody.h"

enum class BridgeSensorType { EnterUpper, LeaveUpper };

class BridgeSensor : public Entity {
   private:
    BridgeSensorType typeBridgeSensor;
    std::shared_ptr<IPhysicalBody> body;

   public:
    BridgeSensor(int id, BridgeSensorType type,
                 std::shared_ptr<IPhysicalBody> body)
        : Entity(id, EntityType::BridgeSensor),
          typeBridgeSensor(type),
          body(body) {}

    BridgeSensorType getType() const { return typeBridgeSensor; }
};
