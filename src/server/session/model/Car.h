#ifndef CAR_H
#define CAR_H

#include <cmath>
#include <memory>


#include <string>

#include "../physics/IPhysicalBody.h"



class Car {
private:
    int id;
    std::string name;
    float acceleration;
    float control;
    float health;
    float nitroMultiplier;
    float maxSpeed;
    bool nitroActive{false};

    std::shared_ptr<IPhysicalBody> body;

public:
    Car(int id, const std::string& name, std::shared_ptr<IPhysicalBody> body,
        float acceleration, float control, float health,
        float nitroMultiplier, float maxSpeed)
        : id(id), name(name),
          acceleration(acceleration), control(control), health(health),
          nitroMultiplier(nitroMultiplier), maxSpeed(maxSpeed) ,body(std::move(body)){}

    int getId() const { return id; }
    float getHealth() const { return health; }
    Vec2 getPosition() const { return body->getPosition(); }
    float getAngle() const { return body->getAngle(); }
    Vec2 getVelocity() const { return body->getLinearVelocity(); }
    void accelerate(float dt) {
        float force = acceleration * (nitroActive ? nitroMultiplier : 1.0f);
        Vec2 dir = { std::cos(getAngle()), std::sin(getAngle()) };
        body->applyForce(dir.x * force * dt, dir.y * force * dt);

        // Limitar la velocidad máxima
        Vec2 vel = body->getLinearVelocity();
        float speed = std::sqrt(vel.x * vel.x + vel.y * vel.y);
        if (speed > maxSpeed) {
            float scale = maxSpeed / speed;
            body->setLinearVelocity(vel.x * scale, vel.y * scale);
        }
    }

    void brake(float dt) {
        Vec2 vel = body->getLinearVelocity();
        body->applyForce(-vel.x * 8.0f * dt, -vel.y * 8.0f * dt);
    }

    void turnLeft(float dt) {
        float newAngle = getAngle() - control * dt;
        body->setTransform(getPosition().x, getPosition().y, newAngle);
    }

    void turnRight(float dt) {
        float newAngle = getAngle() + control * dt;
        body->setTransform(getPosition().x, getPosition().y, newAngle);
    }

    void damage(float amount) { health = std::max(0.0f, health - amount); }
    void activateNitro(bool active) { nitroActive = active; }
    bool isDestroyed() const { return health <= 0; }
    bool isNitroActive() const { return nitroActive; }
};

#endif
