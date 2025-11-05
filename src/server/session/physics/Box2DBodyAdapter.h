

#ifndef TALLER_TP_BOX2DBODYADAPTER_H
#define TALLER_TP_BOX2DBODYADAPTER_H
#include <memory>

#include "IPhysicalBody.h"
#include "Box2dPhysicsBody.h"

//TODO: Adaptador para BOX2D de un cuerpo fisico
class Box2DBodyAdapter : public IPhysicalBody {
private:
    std::unique_ptr<Box2dPhysicsBody> body;
public:
    explicit Box2DBodyAdapter(std::unique_ptr<Box2dPhysicsBody> b)
        : body(std::move(b)) {}

    Vec2 getPosition() const override {
        b2Transform transform = b2Body_GetTransform(body->getId());
        return { transform.p.x, transform.p.y };
    }

    float getAngle() const override {
        b2Transform transform = b2Body_GetTransform(body->getId());
        return b2Rot_GetAngle(transform.q);
    }

    void setTransform(float x, float y, float angle) override {
        b2Rot rot = b2MakeRot(angle);
        b2Body_SetTransform(body->getId(), {x, y}, rot);
    }

    void applyForce(float fx, float fy) override {
        b2Body_ApplyForceToCenter(body->getId(), {fx, fy}, true);
    }

    void setLinearVelocity(float vx, float vy) override {
        b2Body_SetLinearVelocity(body->getId(), {vx, vy});
    }

    Vec2 getLinearVelocity() const override {
        b2Vec2 vel = b2Body_GetLinearVelocity(body->getId());
        return { vel.x, vel.y };
    }

    b2BodyId getId() const override { return body->getId(); }
};

#endif //TALLER_TP_BOX2DBODYADAPTER_H