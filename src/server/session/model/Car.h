#ifndef CAR_H
#define CAR_H

#include <cmath>
#include <iostream>
#include <memory>


#include <string>
#include "Entity.h"
#include "../physics/IPhysicalBody.h"



class Car : public Entity{
private:
    int id;
    std::string name;
    bool accelerating{false};
    bool braking{false};
    std::string turning{false};
    bool nitroActive{false};
    const CarType* type;
    float health;
    std::shared_ptr<IPhysicalBody> body;

public:
    Car(int id, const std::string& name, const CarType& type,std::shared_ptr<IPhysicalBody> body)
        : id(id), name(name), type(&type), health(type.health),body(std::move(body))
        {}

    int getId() const { return id; }
    const CarType& getType() const { return *type; } // acceso seguro
    Vec2 getPosition() const { return body->getPosition(); }
    float getAngle() const { return body->getAngle(); }
    Vec2 getVelocity() const { return body->getLinearVelocity(); }
    void setInput(bool accel, bool brake, const std::string& turn, bool nitro) {
        accelerating = accel;
        braking = brake;
        turning = turn;
        nitroActive = nitro;
    }

    bool isAccelerating() const { return accelerating; }
    bool isBraking() const { return braking; }
    void accelerate() {

        float force = type->acceleration * (nitroActive ? type->nitroMultiplier : 1.0f);
        Vec2 dir = { std::cos(getAngle()), std::sin(getAngle()) };
        body->applyForce(dir.x * force , dir.y * force);

        // Limitar la velocidad máxima
        Vec2 vel = body->getLinearVelocity();
        float speed = std::sqrt(vel.x * vel.x + vel.y * vel.y);
        if (speed > type->maxSpeed) {
            float scale = type->maxSpeed / speed;
            body->setLinearVelocity(vel.x * scale, vel.y * scale);
        }
    }

    void brake() {
        Vec2 dir = { std::cos(getAngle()), std::sin(getAngle()) };
        float force = type->acceleration * 0.5f; // la mitad que acelerar hacia adelante
        body->applyForce(-dir.x * force , -dir.y * force);
    }

    void turnLeft() {
        body->applyTorque(-type->control*20.0f);
    }

    void turnRight() {
        body->applyTorque(type->control*20.0f);
    }
    float getHealth() const { return health; }
    void damage(float amount) {
        health = std::max(0.0f, health - amount);
    }
    void activateNitro(bool active) { nitroActive = active; }
    bool isDestroyed() const { return type->health <= 0; }
    bool isNitroActive() const { return nitroActive; }
    void update() {
        if (accelerating) accelerate();
        if (braking) brake();
        if (turning == "left") { turnLeft();
        } else if (turning == "right") { turnRight();
}
    }
    EntityType getEntityType() const override {
        return EntityType::Car;
    }
};

#endif
