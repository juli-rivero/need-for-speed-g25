#include "CollisionManager.h"

#include <cmath>

#include "../model/Car.h"
#include "../model/Checkpoint.h"
#include "../model/Wall.h"

void CollisionManager::handleHitEvents(const b2ContactEvents& events) {
    std::cout << "[CollisionManager] hitCount = " << events.hitCount
              << std::endl;
    for (int i = 0; i < events.hitCount; ++i) {
        const b2ContactHitEvent& ev = events.hitEvents[i];
        b2ShapeId shapeA = ev.shapeIdA;
        b2ShapeId shapeB = ev.shapeIdB;
        float impact = ev.approachSpeed;

        auto itA = shapeToEntity.find(shapeA);  // Buscar en shapeToEntity
        auto itB = shapeToEntity.find(shapeB);  // Buscar en shapeToEntity
        if (itA == shapeToEntity.end() || itB == shapeToEntity.end()) continue;

        void* entA = itA->second;
        void* entB = itB->second;

        resolvePhysicalImpact(entA, entB, impact);
    }
}
void CollisionManager::resolvePhysicalImpact(void* a, void* b, float impact) {
    auto* entA = static_cast<Entity*>(a);
    auto* entB = static_cast<Entity*>(b);
    if (!entA || !entB) return;

    if (entA->getEntityType() == EntityType::Car &&
        entB->getEntityType() == EntityType::Wall) {
        auto* car = static_cast<Car*>(entA);
        car->damage(impact * 0.5f);
        return;
    }

    if (entA->getEntityType() == EntityType::Wall &&
        entB->getEntityType() == EntityType::Car) {
        auto* car = static_cast<Car*>(entB);
        car->damage(impact * 0.5f);
        return;
    }

    if (entA->getEntityType() == EntityType::Car &&
        entB->getEntityType() == EntityType::Car) {
        auto* car1 = static_cast<Car*>(entA);
        auto* car2 = static_cast<Car*>(entB);
        car1->damage(impact * 0.5f);
        car2->damage(impact * 0.5f);
    }
}
// void CollisionManager::handleBeginTouch(const b2ContactEvents& events) {
//     std::cout << "[CollisionManager] beginTouchCount = " <<
//     events.beginCount<<std::endl; for (int i = 0; i < events.beginCount; ++i)
//     {
//         const b2ContactBeginTouchEvent& ev = events.beginEvents[i];
//         b2BodyId bodyA = b2Shape_GetBody(ev.shapeIdA);
//         b2BodyId bodyB = b2Shape_GetBody(ev.shapeIdB);
//
//         auto itA = bodyToEntity.find(bodyA);
//         auto itB = bodyToEntity.find(bodyB);
//         if (itA == bodyToEntity.end() || itB == bodyToEntity.end()) continue;
//
//         const Car* car = nullptr;
//         const Checkpoint* cp = nullptr;
//         const Hint* hint = nullptr;
//
//         // reconoce tipos
//         if (auto c = dynamic_cast<Car*>(itA->second)) {
//             car = c;
//         }
//         if (auto c = dynamic_cast<Car*>(itB->second)) {
//             car = c;
//         }
//         if (auto ch = dynamic_cast<Checkpoint*>(itA->second)) {
//             cp = ch;
//         }
//         if (auto ch = dynamic_cast<Checkpoint*>(itB->second)) {
//             cp = ch;
//         }
//
//         if (auto h = dynamic_cast<Hint*>(itA->second)) {
//             hint = h;
//         }
//         if (auto h = dynamic_cast<Hint*>(itB->second)) {
//             hint = h;
//         }
//
//         if (car && cp && raceSession) {
//             raceSession->onCheckpointCrossed(car->getId(), cp->getOrder());
//             std::cout << "Car<" << car->getId()
//                       << "> pasó checkpoint #" << cp->getOrder() <<
//                       std::endl;
//         }
//
//         if (car && hint) {
//             std::cout << " Car<" << car->getId() << "> passed hint"
//                       << std::endl;
//         }
//     }
// }
/*void CollisionManager::handleEndTouch(const b2ContactEvents& events) {
    for (int i = 0; i < events.endCount; ++i) {
        const b2ContactEndTouchEvent& ev = events.endEvents[i];
        // similar a begin, pero para salida
    }
}*/
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
        }
    }
}

void CollisionManager::process(const b2ContactEvents& events) {
    handleHitEvents(events);
    // handleBeginTouch(events);
    //  handleEndTouch(events);
}
