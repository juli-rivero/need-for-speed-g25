#pragma once

#include <memory>

struct Vec2 {
    float x, y;
};

// Esta intefaz es para representar un cuerpo fisico
// independientemente del motor.

class IPhysicalBody {
   public:
    virtual ~IPhysicalBody() = default;

    virtual Vec2 getPosition() const = 0;
    virtual float getAngle() const = 0;

    virtual void setTransform(float x, float y, float angle) = 0;
    virtual void applyForce(float fx, float fy) = 0;
    virtual void applyTorque(float torque) = 0;
    virtual void setAngularVelocity(float av) = 0;
    virtual void setLinearVelocity(float vx, float vy) = 0;
    virtual Vec2 getLinearVelocity() const = 0;
    virtual void setCollisionFilter(uint16_t categoryBits,
                                    uint16_t maskBits) = 0;
};
