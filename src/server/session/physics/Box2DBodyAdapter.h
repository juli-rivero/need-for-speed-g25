#pragma once

#include <memory>
#include <utility>

#include "Box2dPhysicsBody.h"
#include "IPhysicalBody.h"

// Adaptador para BOX2D de un cuerpo fisico
class Box2DBodyAdapter : public IPhysicalBody {
   private:
    std::shared_ptr<Box2dPhysicsBody> body;

   public:
    explicit Box2DBodyAdapter(std::shared_ptr<Box2dPhysicsBody> b)
        : body(std::move(b)) {}

    Vec2 getPosition() const override {
        b2Transform transform = b2Body_GetTransform(body->getId());
        return {transform.p.x, transform.p.y};
    }

    float getAngle() const override {
        b2Transform transform = b2Body_GetTransform(body->getId());
        return b2Rot_GetAngle(transform.q);
    }
    b2ShapeId getShapeId() const { return body->getShapeId(); }
    void setTransform(float x, float y, float angle) override {
        b2Rot rot = b2MakeRot(angle);
        b2Body_SetTransform(body->getId(), {x, y}, rot);
    }

    void applyForce(float fx, float fy) override {
        b2Body_ApplyForceToCenter(body->getId(), {fx, fy}, true);
    }
    void applyTorque(float torque) override {
        b2Body_ApplyTorque(body->getId(), torque, true);
    }
    void setAngularVelocity(float av) override {
        b2Body_SetAngularVelocity(body->getId(), av);
    }

    void setLinearVelocity(float vx, float vy) override {
        b2Body_SetLinearVelocity(body->getId(), {vx, vy});
    }

    Vec2 getLinearVelocity() const override {
        b2Vec2 vel = b2Body_GetLinearVelocity(body->getId());
        return {vel.x, vel.y};
    }
    void setCollisionFilter(uint16_t categoryBits, uint16_t maskBits) override {
        b2ShapeId shape = body->getShapeId();
        b2Filter filter = b2Shape_GetFilter(shape);
        filter.categoryBits = categoryBits;
        filter.maskBits = maskBits;
        b2Shape_SetFilter(shape, filter);
    }
    b2BodyId getId() const { return body->getId(); }
};
