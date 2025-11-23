#include "CollisionManager.h"

#include <cmath>
#include <iostream>

#include "../model/Car.h"
#include "../model/Checkpoint.h"
#include "../model/Wall.h"
#include "server/session/model/BridgeSensor.h"

void CollisionManager::generateCollisionEvent(Entity* entA, Entity* entB,
                                              float impact) {
    if (entA->getEntityType() == EntityType::Car &&
        entB->getEntityType() == EntityType::Wall) {
        auto* car = static_cast<Car*>(entA);
        auto it = carToPlayer.find(car);
        if (it != carToPlayer.end()) {
            collisionEvents.push_back(CollisionSimple{it->second, impact});
        }
        return;
    }

    if (entA->getEntityType() == EntityType::Wall &&
        entB->getEntityType() == EntityType::Car) {
        auto* car = static_cast<Car*>(entB);
        auto it = carToPlayer.find(car);
        if (it != carToPlayer.end()) {
            collisionEvents.push_back(CollisionSimple{it->second, impact});
        }
        return;
    }

    if (entA->getEntityType() == EntityType::Car &&
        entB->getEntityType() == EntityType::Car) {
        auto* car1 = static_cast<Car*>(entA);
        auto* car2 = static_cast<Car*>(entB);

        auto it1 = carToPlayer.find(car1);
        auto it2 = carToPlayer.find(car2);

        if (it1 != carToPlayer.end() && it2 != carToPlayer.end()) {
            collisionEvents.push_back(CollisionCarToCar{
                it1->second,
                it2->second,
                impact,
            });
        }
    }
}
void CollisionManager::handleHitEvents(const b2ContactEvents& events) {
    for (int i = 0; i < events.hitCount; ++i) {
        const b2ContactHitEvent& ev = events.hitEvents[i];
        b2ShapeId shapeA = ev.shapeIdA;
        b2ShapeId shapeB = ev.shapeIdB;
        float impact = ev.approachSpeed;

        auto itA = shapeToEntity.find(shapeA);
        auto itB = shapeToEntity.find(shapeB);
        if (itA == shapeToEntity.end() || itB == shapeToEntity.end()) continue;

        Entity* entA = itA->second;
        Entity* entB = itB->second;

        resolvePhysicalImpact(entA, entB, impact);
        generateCollisionEvent(entA, entB, impact);
    }
}

void CollisionManager::resolvePhysicalImpact(Entity* a, Entity* b,
                                             float impact) {
    if (!a || !b) return;

    if (a->getEntityType() == EntityType::Car &&
        b->getEntityType() == EntityType::Wall) {
        auto* car = static_cast<Car*>(a);
        car->damage(impact * 0.5f);
        return;
    }

    if (a->getEntityType() == EntityType::Wall &&
        b->getEntityType() == EntityType::Car) {
        auto* car = static_cast<Car*>(b);
        car->damage(impact * 0.5f);
        return;
    }

    if (a->getEntityType() == EntityType::Car &&
        b->getEntityType() == EntityType::Car) {
        auto* car1 = static_cast<Car*>(a);
        auto* car2 = static_cast<Car*>(b);
        car1->damage(impact * 0.5f);
        car2->damage(impact * 0.5f);
    }
}

void CollisionManager::processSensors(const b2SensorEvents& events) {
    for (int i = 0; i < events.beginCount; ++i) {
        const b2SensorBeginTouchEvent& ev = events.beginEvents[i];
        b2ShapeId sA = ev.sensorShapeId;
        b2ShapeId sB = ev.visitorShapeId;

        auto itA = shapeToEntity.find(sA);
        auto itB = shapeToEntity.find(sB);
        if (itA == shapeToEntity.end() || itB == shapeToEntity.end()) continue;

        Entity* eA = itA->second;
        Entity* eB = itB->second;

        // CHECKPOINTS
        if (eA->getEntityType() == EntityType::Checkpoint &&
            eB->getEntityType() == EntityType::Car) {
            auto* car = static_cast<Car*>(eB);
            const auto* cp = static_cast<Checkpoint*>(eA);
            auto itOwner = carToPlayer.find(car);
            if (raceSession && itOwner != carToPlayer.end()) {
                PlayerId pid = itOwner->second;
                raceSession->onCheckpointCrossed(pid, cp->getOrder());
            }
        } else if (eA->getEntityType() == EntityType::Car &&
                   eB->getEntityType() == EntityType::Checkpoint) {
            auto* car = static_cast<Car*>(eA);
            const auto* cp = static_cast<Checkpoint*>(eB);
            auto itOwner = carToPlayer.find(car);
            if (raceSession && itOwner != carToPlayer.end()) {
                PlayerId pid = itOwner->second;
                raceSession->onCheckpointCrossed(pid, cp->getOrder());
            }
        } else if (eA->getEntityType() == EntityType::BridgeSensor &&
                   eB->getEntityType() == EntityType::Car) {
            auto* sensor = static_cast<BridgeSensor*>(eA);
            auto* car = static_cast<Car*>(eB);

            if (sensor->getType() == BridgeSensorType::SetUpper) {
                car->setLayer(RenderLayer::OVER);
                std::cout << "[DEBUG] Car " << car->getId()
                          << " → Set LAYER = OVER (upper sensor)\n";
            } else if (sensor->getType() == BridgeSensorType::SetLower) {
                car->setLayer(RenderLayer::UNDER);
                std::cout << "[DEBUG] Car " << car->getId()
                          << " → Set LAYER = UNDER (lower sensor)\n";
            }
        } else if (eA->getEntityType() == EntityType::Car &&
                   eB->getEntityType() == EntityType::BridgeSensor) {
            auto* car = static_cast<Car*>(eA);
            auto* sensor = static_cast<BridgeSensor*>(eB);
            if (sensor->getType() == BridgeSensorType::SetUpper) {
                car->setLayer(RenderLayer::OVER);
                std::cout << "[DEBUG] Car " << car->getId()
                          << " → Set LAYER = OVER (upper sensor)\n";
            } else if (sensor->getType() == BridgeSensorType::SetLower) {
                car->setLayer(RenderLayer::UNDER);
                std::cout << "[DEBUG] Car " << car->getId()
                          << " → Set LAYER = UNDER (lower sensor)\n";
            }
        }
    }
}

void CollisionManager::process(const b2ContactEvents& events) {
    handleHitEvents(events);
}
