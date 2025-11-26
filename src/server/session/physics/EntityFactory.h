#pragma once
#include <memory>

#include "../model/Car.h"
#include "../model/Checkpoint.h"
#include "../model/Wall.h"
#include "Box2DPhysicsFactory.h"
#include "Box2DPhysicsWorld.h"
#include "server/session/model/BridgeSensor.h"

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
    std::unique_ptr<Car> createCar(const CarType&, float x, float y);
    std::unique_ptr<Wall> createBuilding(float x, float y, float w, float h,
                                         EntityType type);
    std::unique_ptr<Checkpoint> createCheckpoint(float x, float y, float w,
                                                 float h, float angle,
                                                 int order,
                                                 CheckpointType type);
    std::unique_ptr<BridgeSensor> createBridgeSensor(RenderLayer type, float x,
                                                     float y, float w, float h);
};
