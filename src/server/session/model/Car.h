#pragma once

#include <algorithm>
#include <cmath>
#include <iostream>
#include <memory>
#include <string>
#include <utility>

#include "../physics/IPhysicalBody.h"
#include "Entity.h"
#include "common/structs.h"
#include "server/session/logic/types.h"
#include "server/session/model/CarType.h"
#include "spdlog/spdlog.h"

enum class CarInput {
    StartTurningLeft,
    StopTurningLeft,
    StartTurningRight,
    StopTurningRight,
    StartAccelerating,
    StopAccelerating,
    StartReversing,
    StopReversing,
    StartUsingNitro,
};

struct UpgradeStats {
    float bonusMaxSpeed = 0;
    float bonusAcceleration = 0;
    float bonusHealth = 0;
    float bonusNitro = 0;
};

class Car : public Entity {
   private:
    std::string name;
    RenderLayer layer = RenderLayer::UNDER;

    bool accelerating{false};
    bool braking{false};
    TurnDirection turning{TurnDirection::None};

    bool nitroActive{false};
    float nitroTimeRemaining = 0.0f;
    float nitroCooldownRemaining = 0.0f;

    const CarType& carType;

    float health;
    float maxHealth;

    mutable UpgradeStats upgrades;

    std::shared_ptr<IPhysicalBody> body;

   public:
    Car(int id, const std::string& name, const CarType& type,
        std::shared_ptr<IPhysicalBody> body)
        : Entity(id, EntityType::Car),
          name(name),
          carType(type),
          health(type.maxHealth),
          maxHealth(type.maxHealth),
          body(std::move(body)) {}
    UpgradeStats& getUpgrades() const { return upgrades; }
    const CarType& getType() const { return carType; }  // acceso seguro
    Vec2 getPosition() const { return body->getPosition(); }
    float getAngle() const { return body->getAngle(); }
    Vec2 getVelocity() const { return body->getLinearVelocity(); }
    float getHealth() const { return health; }
    float getMaxHealth() const { return maxHealth; }
    RenderLayer getLayer() const { return layer; }
    void setLayer(RenderLayer l) { layer = l; }
    void setHealth(float h) { health = std::clamp(h, 0.0f, maxHealth); }

    void addHealth(float delta) {
        health = std::clamp(health + delta, 0.0f, maxHealth);
    }

    void setMaxHealth(float h) {
        maxHealth = h;
        if (health > maxHealth) health = maxHealth;
    }

    void increaseMaxHealth(float delta) {
        maxHealth += delta;
        health = std::min(health + delta, maxHealth);
    }

    void applyInput(const CarInput action) {
        switch (action) {
            case CarInput::StartTurningLeft:
                turning = TurnDirection::Left;
                break;
            case CarInput::StopTurningLeft:
                if (turning == TurnDirection::Left)
                    turning = TurnDirection::None;
                break;
            case CarInput::StartTurningRight:
                turning = TurnDirection::Right;
                break;
            case CarInput::StopTurningRight:
                if (turning == TurnDirection::Right)
                    turning = TurnDirection::None;
                break;
            case CarInput::StartAccelerating:
                accelerating = true;
                break;
            case CarInput::StopAccelerating:
                accelerating = false;
                break;
            case CarInput::StartReversing:
                braking = true;
                break;
            case CarInput::StopReversing:
                braking = false;
                break;
            case CarInput::StartUsingNitro:
                nitroActive = !nitroActive;
                break;
            default:
                throw std::runtime_error("Invalid CarActionType");
        }
    }

    bool isAccelerating() const { return accelerating; }
    bool isBraking() const { return braking; }
    void accelerate() {
        float accel = carType.acceleration + upgrades.bonusAcceleration;
        float force = accel * (nitroActive ? carType.nitroMultiplier : 1.0f);
        Vec2 dir = {std::cos(getAngle()), std::sin(getAngle())};
        body->applyForce(dir.x * force, dir.y * force);

        Vec2 vel = body->getLinearVelocity();
        float speed = std::sqrt(vel.x * vel.x + vel.y * vel.y);
        float maxSpeed = carType.maxSpeed + upgrades.bonusMaxSpeed;
        if (speed > maxSpeed) {
            float scale = carType.maxSpeed / speed;
            body->setLinearVelocity(vel.x * scale, vel.y * scale);
        }
    }

    void brake() {
        Vec2 dir = {std::cos(getAngle()), std::sin(getAngle())};
        float force = carType.acceleration *
                      0.5f;  // la mitad que acelerar hacia adelante
        body->applyForce(-dir.x * force, -dir.y * force);
    }

    void turnLeft() { body->applyTorque(-carType.control * 20.0f); }

    void turnRight() { body->applyTorque(carType.control * 20.0f); }

    void damage(float amount) { setHealth(health - amount); }
    void activateNitro(bool active) {
        if (active) {
            if (!nitroActive && nitroCooldownRemaining <= 0.0f) {
                nitroActive = true;
                nitroTimeRemaining = carType.nitroDuration;
            }
        } else {
            nitroActive = false;
        }
    }
    bool isDestroyed() const { return health <= 0; }
    bool isNitroActive() const { return nitroActive; }
    void killLateralVelocity() {
        Vec2 vel = body->getLinearVelocity();
        Vec2 rightNormal = {-std::sin(getAngle()), std::cos(getAngle())};

        float lateralSpeed = vel.x * rightNormal.x + vel.y * rightNormal.y;

        Vec2 lateralVel = {rightNormal.x * lateralSpeed,
                           rightNormal.y * lateralSpeed};

        body->setLinearVelocity(vel.x - lateralVel.x, vel.y - lateralVel.y);
    }
    void turn() {
        // la magnitud de giro depende de la velocidad
        Vec2 vel = body->getLinearVelocity();
        float speed = std::sqrt(vel.x * vel.x + vel.y * vel.y);

        if (speed < 0.5f) return;  // NO girasi está casi quieto

        float steerStrength = carType.control;
        float turnRate = steerStrength * (speed / carType.maxSpeed);

        if (turning == TurnDirection::Left)
            body->setAngularVelocity(-turnRate * 5.0f);
        else if (turning == TurnDirection::Right)
            body->setAngularVelocity(turnRate * 5.0f);
        else
            body->setAngularVelocity(0.0f);
    }
    void update(float dt) {
        if (nitroActive) {
            nitroTimeRemaining -= dt;
            if (nitroTimeRemaining <= 0.0f) {
                nitroActive = false;
                nitroCooldownRemaining = carType.nitroCooldown;
            }
        } else {
            if (nitroCooldownRemaining > 0.0f) {
                nitroCooldownRemaining -= dt;
            }
        }

        if (accelerating) accelerate();
        if (braking) brake();

        killLateralVelocity();
        // para simular traccion delantera de un auto
        turn();
    }
};
