//
// Created by elvis on 21/10/25.
//

#ifndef NEED_FOR_SPEED_G25_PHYSICSWORLD_H
#define NEED_FOR_SPEED_G25_PHYSICSWORLD_H


#include <box2d/box2d.h>
#include <memory>
#include <vector>

#include "CollisionManager.h"

//WRAPPER DEL MUNDO
class Box2DPhysicsWorld {
private:
    b2WorldId world;
    std::unique_ptr<CollisionManager> collisionManager;
public:
    Box2DPhysicsWorld();
    ~Box2DPhysicsWorld();

    void step(float dt);
    b2WorldId getWorldId() const { return world; }
    CollisionManager& getCollisionManager() { return *collisionManager; }


};

#endif //NEED_FOR_SPEED_G25_PHYSICSWORLD_H