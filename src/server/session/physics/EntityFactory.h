#pragma once
#include <memory>

#include "Box2DPhysicsFactory.h"
#include "Box2DPhysicsWorld.h"
#include "server/config/YamlGameConfig.h"
#include "server/session/model/BridgeSensor.h"
#include "server/session/model/Car.h"
#include "server/session/model/Checkpoint.h"
#include "server/session/model/Wall.h"

class EntityFactory {
    Box2DPhysicsWorld& world;
    const YamlGameConfig& cfg;
    Box2DPhysicsFactory physicsFactory;

    static int nextId() {
        static int counter = 0;
        return counter++;
    }

   public:
    EntityFactory(Box2DPhysicsWorld&, const YamlGameConfig&);

    // Todos reciben una referencia al mundo físico
    std::unique_ptr<Car> createCar(const CarType&, const Point& pos,
                                   float angleDeg, EntityType entType);
    std::unique_ptr<Car> createNpcCar(CarType type, const Point& pos);
    std::unique_ptr<Wall> createBuilding(const Bound&, EntityType type);
    std::unique_ptr<Checkpoint> createCheckpoint(const CheckpointInfo&);
    std::unique_ptr<BridgeSensor> createBridgeSensor(RenderLayer type,
                                                     const Bound&);
};
