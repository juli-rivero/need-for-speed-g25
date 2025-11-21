#pragma once
#include <memory>

#include "../model/Bridge.h"
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
    std::unique_ptr<Wall> createWall(float x, float y, float w, float h);
    std::unique_ptr<Checkpoint> createCheckpoint(float x, float y, float w,
                                                 float h, float angle,
                                                 int order);
    std::unique_ptr<Bridge> createBridge(float x, float y, float w, float h,
                                         bool driveable);
    // std::unique_ptr<BridgeSensor> createBridgeSensor(Box2DPhysicsWorld&
    // world,
    //                                                  BridgeSensorType type,
    //                                                  float x, float y, float
    //                                                  w, float h);
};
