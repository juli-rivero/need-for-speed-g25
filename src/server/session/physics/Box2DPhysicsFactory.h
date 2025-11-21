#pragma once

#include <memory>

#include "Box2dPhysicsBody.h"
#include "common/structs.h"

// FABRICA DE CUERPOS FISICOS DE BOX2D
class Box2DPhysicsFactory {
    b2WorldId world;

   public:
    explicit Box2DPhysicsFactory(b2WorldId world);

    std::shared_ptr<Box2dPhysicsBody> createCar(float x, float y,
                                                const CarStaticStats& type);
    std::shared_ptr<Box2dPhysicsBody> createBuilding(float x, float y, float w,
                                                     float h);
    std::shared_ptr<Box2dPhysicsBody> createCheckpoint(float x, float y,
                                                       float w, float h);
    std::shared_ptr<Box2dPhysicsBody> createBridge(float x, float y, float w,
                                                   float h, bool driveable);
};
