#pragma once

#include <algorithm>
#include <cmath>
#include <iostream>
#include <memory>
#include <string>
#include <utility>

#include "../physics/IPhysicalBody.h"
#include "Entity.h"
#include "box2d/box2d.h"
#include "box2d/types.h"
#include "common/structs.h"
#include "server/session/logic/types.h"
#include "server/session/physics/IPhysicalBody.h"
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

class Car : public Entity {
   private:
    RenderLayer layer = RenderLayer::UNDER;

    bool accelerating{false};
    bool braking{false};
    TurnDirection turning{TurnDirection::None};

    bool nitroActive{false};
    float nitroTimeRemaining = 0.0f;
    float nitroCooldownRemaining = 0.0f;

    const CarStaticStats staticStats;

    float health;

    mutable UpgradeStats upgrades;

    std::shared_ptr<IPhysicalBody> body;

    const CarType carType;

   public:
    Car(const int id, const CarType carType, const CarStaticStats& staticStats,
        std::shared_ptr<IPhysicalBody> body, EntityType entType)
        : Entity(id, entType),
          staticStats(staticStats),
          health(staticStats.maxHealth),
          body(std::move(body)),
          carType(carType) {}
    RenderLayer getLayer() const { return layer; }
    static constexpr EntityType Type = EntityType::Car;
    std::shared_ptr<IPhysicalBody> getBody() { return body; }
    CarType getType() const { return carType; }
    UpgradeStats getUpgrades() const { return upgrades; }
    void setLayer(RenderLayer l) {
        layer = l;

        b2ShapeId shape = body->getShapeId();
        b2Filter filter = b2Shape_GetFilter(shape);

        bool isNPC = (getEntityType() == EntityType::NPCCar);
        filter.categoryBits = isNPC ? CATEGORY_NPC : CATEGORY_CAR;

        // Siempre paredes + sensores
        filter.maskBits = CATEGORY_WALL | CATEGORY_SENSOR;

        //  REGLA DE COLISIÓN POR CAPA
        if (layer == RenderLayer::UNDER) {
            if (!isNPC) {
                filter.maskBits |= CATEGORY_CAR;
                filter.maskBits |= CATEGORY_NPC;
            } else {
                filter.maskBits |= CATEGORY_CAR;
            }

        } else {
            filter.maskBits |= CATEGORY_RAILING;
            if (!isNPC) {
                filter.maskBits |= CATEGORY_CAR;
                filter.maskBits |= CATEGORY_NPC;
            } else {
                filter.maskBits |= CATEGORY_CAR;
            }
        }
        b2Shape_SetFilter(shape, filter);
    }

   private:
    Vec2 getDirection() const {
        return {std::cos(body->getAngle()), std::sin(body->getAngle())};
    }

    void accelerate() {
        float accel = staticStats.acceleration + upgrades.bonusAcceleration;
        float force =
            accel * (nitroActive ? staticStats.nitroMultiplier : 1.0f);
        Vec2 dir = getDirection();
        body->applyForce(dir.x * force, dir.y * force);

        Vec2 vel = body->getLinearVelocity();
        float speed = std::sqrt(vel.x * vel.x + vel.y * vel.y);
        float maxSpeed = staticStats.maxSpeed + upgrades.bonusMaxSpeed;
        if (speed > maxSpeed) {
            float scale = staticStats.maxSpeed / speed;
            body->setLinearVelocity(vel.x * scale, vel.y * scale);
        }
    }

    void brake() {
        Vec2 dir = getDirection();
        float force = staticStats.acceleration *
                      0.5f;  // la mitad que acelerar hacia adelante
        body->applyForce(-dir.x * force, -dir.y * force);
    }

    void activateNitro() {
        if (!nitroActive && nitroCooldownRemaining <= 0.0f) {
            nitroActive = true;
            nitroTimeRemaining = staticStats.nitroDuration;
        }
    }

    void killLateralVelocity() {
        Vec2 vel = body->getLinearVelocity();
        float speed = std::sqrt(vel.x * vel.x + vel.y * vel.y);
        float maxSpeed = staticStats.maxSpeed + upgrades.bonusMaxSpeed;

        // dirección del auto
        Vec2 dir = getDirection();
        Vec2 lateralNormal = {-dir.y, dir.x};

        // proyección lateral
        float lateralSpeed = vel.x * lateralNormal.x + vel.y * lateralNormal.y;

        // cuando estás quieto: NO derrapa nada
        // a máxima velocidad: derrape leve (configurado por YAML)
        float t = std::clamp(speed / maxSpeed, 0.0f, 1.0f);

        // interpolación:
        // quieto -> menos slip (0.98)
        // rápido -> driftFactor (0.90)
        float slip =
            std::lerp(0.98f, static_cast<float>(staticStats.driftFactor), t);

        Vec2 lateralVel = {lateralNormal.x * lateralSpeed * slip,
                           lateralNormal.y * lateralSpeed * slip};

        body->setLinearVelocity(vel.x - lateralVel.x, vel.y - lateralVel.y);
    }

    void turn() {
        Vec2 vel = body->getLinearVelocity();
        float speed = std::sqrt(vel.x * vel.x + vel.y * vel.y);

        // No gira en el lugar
        // Para simular tracción delantera: si el auto está casi quieto, NO
        // gira.
        if (speed < 2.0f) {  // podés ajustar 1.5f / 2.0f según gusto
            body->setAngularVelocity(0.0f);
            return;
        }

        // A partir de aquí, velocidad suficiente para girar
        float speedNorm =
            std::clamp(speed / (staticStats.maxSpeed + 0.1f), 0.0f, 1.0f);

        // Curva suave: menos giro a alta velocidad, más en media velocidad
        float speedFactor = 0.3 * (1.0f - speedNorm);

        float turnRate = staticStats.control * speedFactor;

        if (turning == TurnDirection::Left)
            body->setAngularVelocity(-turnRate);
        else if (turning == TurnDirection::Right)
            body->setAngularVelocity(turnRate);
        else
            body->setAngularVelocity(0.0f);
    }

    void updateNitro(const float dt) {
        if (nitroActive) nitroTimeRemaining -= dt;
        if (nitroActive && nitroTimeRemaining <= 0.0f) {
            nitroActive = false;
            nitroCooldownRemaining = staticStats.nitroCooldown;
        }
        if (nitroCooldownRemaining >= 0.0f) {
            nitroCooldownRemaining -= dt;
        }
    }

   public:
    void damage(float amount) { health -= std::max(0.0f, amount); }

    bool isDestroyed() const { return health <= 0; }

    void update(float dt) {
        if (isDestroyed()) {
            body->setLinearVelocity(0.0, 0.0);
            body->setAngularVelocity(0);
            return;
        }

        updateNitro(dt);

        if (accelerating) accelerate();
        if (braking) brake();

        killLateralVelocity();
        // para simular traccion delantera de un auto
        turn();
    }

    const CarStaticStats& getStaticStats() const { return staticStats; }

    CarSnapshot getSnapshot() const {
        const auto [x, y] = body->getPosition();
        const auto [vx, vy] = body->getLinearVelocity();
        return {
            .type = carType,
            .layer = layer,
            .bound = {Point{x, y}, staticStats.width, staticStats.height},
            .vx = vx,
            .vy = vy,
            .angle = body->getAngle(),
            .speed = std::sqrt(vx * vx + vy * vy),
            .health = health,
            .nitroActive = nitroActive,
            .braking = braking,
            .accelerating = accelerating,
        };
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
                activateNitro();
                break;
            default:
                throw std::runtime_error("Invalid CarActionType");
        }
    }

    void upgrade(const UpgradeStat up, float amount) {
        switch (up) {
            case UpgradeStat::MaxSpeed:
                upgrades.bonusMaxSpeed += amount;
                break;

            case UpgradeStat::Acceleration:
                upgrades.bonusAcceleration += amount;
                break;

            case UpgradeStat::Health:
                upgrades.bonusHealth += amount;
                health += amount;  // TODO(elvis): no se resetea la vida en
                                   // cada carrera?
                break;

            case UpgradeStat::Nitro:
                upgrades.bonusNitro += amount;
                break;
        }
    }

    void setTransform(const Point pos, const float angle) {
        body->setTransform(pos.x, pos.y, angle);
    }
};
