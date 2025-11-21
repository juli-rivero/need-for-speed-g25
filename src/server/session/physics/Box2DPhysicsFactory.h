#pragma once

#include <memory>

#include "../model/CarType.h"
#include "Box2dPhysicsBody.h"

// FABRICA DE CUERPOS FISICOS DE BOX2D
class Box2DPhysicsFactory {
   public:
    static std::shared_ptr<Box2dPhysicsBody> createCar(b2WorldId world, float x,
                                                       float y,
                                                       const CarType& type);
    static std::shared_ptr<Box2dPhysicsBody> createBuilding(b2WorldId world,
                                                            float x, float y,
                                                            float w, float h);
    static std::shared_ptr<Box2dPhysicsBody> createCheckpoint(b2WorldId world,
                                                              float x, float y,
                                                              float w, float h);
    static std::shared_ptr<Box2dPhysicsBody> createBridge(b2WorldId world,
                                                          float x, float y,
                                                          float w, float h,
                                                          bool driveable);

    static std::shared_ptr<Box2dPhysicsBody> createBridgeSensor(
        b2WorldId world, float x, float y, float w, float h);
};
