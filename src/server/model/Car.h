#ifndef CAR_H
#define CAR_H

#include <cmath>
#include <memory>


#include <string>

#include "../physics/IPhysicalBody.h"

class IPhysicalBody;

class Car {
private:
    int id;
    std::string color;
    float acceleration{10.0f};
    float health{100.0f};

    std::shared_ptr<IPhysicalBody> body;

public:
    Car(int id, const std::string& color, std::shared_ptr<IPhysicalBody> body)
        : id(id), color(color), body(std::move(body)) {}

    int getId() const { return id; }
    const std::string& getColor() const { return color; }

    Vec2 getPosition() const { return body->getPosition(); }
    float getAngle() const { return body->getAngle(); }
    Vec2 getVelocity() const { return body->getLinearVelocity(); }

    void accelerate(float dt) {
        Vec2 vel = body->getLinearVelocity();
        float vx = std::cos(getAngle()) * acceleration * dt;
        float vy = std::sin(getAngle()) * acceleration * dt;
        body->applyForce(vx, vy);
    }

    void damage(float amount) {
        health = std::max(0.0f, health - amount);
    }

    bool isDestroyed() const { return health <= 0; }
};

#endif
