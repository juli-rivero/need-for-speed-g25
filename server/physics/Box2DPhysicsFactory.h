

#ifndef NEED_FOR_SPEED_G25_PHYSICSFACTORY_H
#define NEED_FOR_SPEED_G25_PHYSICSFACTORY_H


#include "Box2dPhysicsBody.h"
#include <memory>
//FABRICA DE CUERPOS FISICOS DE BOX2D
class Box2DPhysicsFactory {
public:
    static std::unique_ptr<Box2dPhysicsBody> createCar(b2WorldId world, float x, float y);
    static std::unique_ptr<Box2dPhysicsBody> createBuilding(b2WorldId world, float x, float y, float w, float h);
    static std::unique_ptr<Box2dPhysicsBody> createCheckpoint(b2WorldId world, float x, float y, float w,float h);
    static std::unique_ptr<Box2dPhysicsBody> createHint(b2WorldId world, float x, float y, float w, float h);
    static std::unique_ptr<Box2dPhysicsBody> createBridge(b2WorldId world, float x, float y, float w, float h, bool driveable);
};


#endif //NEED_FOR_SPEED_G25_PHYSICSFACTORY_H