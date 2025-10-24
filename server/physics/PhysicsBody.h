//
// Created by elvis on 21/10/25.
//

#ifndef NEED_FOR_SPEED_G25_PHYSICSBODY_H
#define NEED_FOR_SPEED_G25_PHYSICSBODY_H


#include <box2d/box2d.h>
//WRAPPER DE UN CUERPO FISICO
class PhysicsBody {
public:
    PhysicsBody(b2WorldId world, const b2BodyDef& def)
        : world(world) {
        body = b2CreateBody(world, &def);
    }

    ~PhysicsBody() {
        if (b2Body_IsValid(body))
            b2DestroyBody(body);
    }

    b2BodyId getId() const { return body; }

    void setTransform(float x, float y, float angle) {
        b2Body_SetTransform(body, {x, y}, b2MakeRot(angle));
    }

    b2Vec2 getPosition() const {
        return b2Body_GetPosition(body);
    }


private:
    b2WorldId world;
    b2BodyId body;
};
#endif //NEED_FOR_SPEED_G25_PHYSICSBODY_H