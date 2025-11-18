#pragma once
#include <memory>

#include "../model/Bridge.h"
#include "../model/Car.h"
#include "../model/Checkpoint.h"
#include "../model/Wall.h"
#include "Box2DPhysicsWorld.h"

class EntityFactory {
   private:
    static int nextId() {
        static int counter = 0;
        return counter++;
    }

   public:
    // Todos reciben una referencia al mundo físico
    static std::unique_ptr<Car> createCar(Box2DPhysicsWorld& world,
                                          const CarType& type, float x,
                                          float y);
    static std::unique_ptr<Wall> createWall(Box2DPhysicsWorld& world, float x,
                                            float y, float w, float h);
    static std::unique_ptr<Checkpoint> createCheckpoint(
        Box2DPhysicsWorld& world, float x, float y, float w, float h,
        float angle, int order);
    static std::unique_ptr<Bridge> createBridge(Box2DPhysicsWorld& world,
                                                float x, float y, float w,
                                                float h, bool driveable);
};
