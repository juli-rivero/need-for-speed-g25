#pragma once

#include <box2d/box2d.h>
// WRAPPER DE UN CUERPO FISICO
class Box2dPhysicsBody {
   public:
    Box2dPhysicsBody(b2WorldId world, const b2BodyDef& def)
        : world(world), body(b2CreateBody(world, &def)) {}

    ~Box2dPhysicsBody() {
        if (b2Body_IsValid(body)) {
            b2DestroyBody(body);
        }
    }

    b2BodyId getId() const { return body; }

    void setTransform(float x, float y, float angle) {
        b2Body_SetTransform(body, {x, y}, b2MakeRot(angle));
    }

    b2Vec2 getPosition() const { return b2Body_GetPosition(body); }

   private:
    [[maybe_unused]] b2WorldId world;  // TODO(elvis) se usara luego
    b2BodyId body;
};
