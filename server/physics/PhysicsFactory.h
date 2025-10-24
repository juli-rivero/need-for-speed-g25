//
// Created by elvis on 21/10/25.
//

#ifndef NEED_FOR_SPEED_G25_PHYSICSFACTORY_H
#define NEED_FOR_SPEED_G25_PHYSICSFACTORY_H


#include "PhysicsBody.h"
#include <memory>
//FABRICA DE CUERPOS FISICOS DE BOX2D
class PhysicsFactory {
public:
    static std::unique_ptr<PhysicsBody> createCar(b2WorldId world, float x, float y);
    static std::unique_ptr<PhysicsBody> createBuilding(b2WorldId world, float x, float y, float w, float h);
    static std::unique_ptr<PhysicsBody> createCheckpoint(b2WorldId world, float x, float y, float radius);
    static std::unique_ptr<PhysicsBody> createHint(b2WorldId world, float x, float y, float w, float h);
    static std::unique_ptr<PhysicsBody> createBridge(b2WorldId world, float x, float y, float w, float h, bool driveable);
};


#endif //NEED_FOR_SPEED_G25_PHYSICSFACTORY_H