//
// Created by elvis on 21/10/25.
//

#ifndef NEED_FOR_SPEED_G25_PHYSICSWORLD_H
#define NEED_FOR_SPEED_G25_PHYSICSWORLD_H


#include <box2d/box2d.h>
#include <memory>
#include <vector>

//WRAPPER DEL MUNDO
class PhysicsWorld {
private:
    b2WorldId world;
public:
    PhysicsWorld();
    ~PhysicsWorld();

    void step(float dt);
    b2WorldId getWorldId() const { return world; }


};

#endif //NEED_FOR_SPEED_G25_PHYSICSWORLD_H