#include "CollisionManager.h"

#include <cmath>
#include <iostream>

#include "../model/Car.h"
#include "../model/Checkpoint.h"
#include "../model/Wall.h"
#include "server/session/model/BridgeSensor.h"

namespace {

// devuelve true si A es tipo TA y B es tipo TB (sin importar orden)
template <typename TA, typename TB>
bool isPair(const Entity* a, const Entity* b) {
    return (a->getEntityType() == TA::Type && b->getEntityType() == TB::Type);
}

template <typename T>
T* as(Entity* e) {
    return static_cast<T*>(e);
}

}  // namespace

void CollisionManager::generateCollisionEvent(Entity* entA, Entity* entB,
                                              float impact) {
    // Car/ Wall
    if (isPair<Car, Wall>(entA, entB) || isPair<Wall, Car>(entA, entB)) {
        Car* car = (entA->getEntityType() == EntityType::Car) ? as<Car>(entA)
                                                              : as<Car>(entB);
        auto it = carToPlayer.find(car);
        if (it != carToPlayer.end())
            collisionEvents.push_back(CollisionSimple{it->second, impact});
        return;
    }

    // Car / Car
    if (isPair<Car, Car>(entA, entB)) {
        auto* c1 = as<Car>(entA);
        auto* c2 = as<Car>(entB);
        auto it1 = carToPlayer.find(c1);
        auto it2 = carToPlayer.find(c2);

        if (it1 != carToPlayer.end() && it2 != carToPlayer.end()) {
            collisionEvents.push_back(
                CollisionCarToCar{it1->second, it2->second, impact});
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

    // Car ↔ Wall
    if (isPair<Car, Wall>(a, b) || isPair<Wall, Car>(a, b)) {
        Car* car =
            (a->getEntityType() == EntityType::Car) ? as<Car>(a) : as<Car>(b);
        car->damage(impact * 0.5f);
        return;
    }

    // Car ↔ Car
    if (isPair<Car, Car>(a, b)) {
        as<Car>(a)->damage(impact * 0.5f);
        as<Car>(b)->damage(impact * 0.5f);
    }
}
static void handleCheckpointTouch(
    Entity* cp, Entity* car, const RaceSession* race,
    const std::unordered_map<const Car*, PlayerId>& carToPlayer) {
    const auto* checkpoint = as<Checkpoint>(cp);
    auto* c = as<Car>(car);
    auto it = carToPlayer.find(c);

    if (race && it != carToPlayer.end()) {
        race->onCheckpointCrossed(it->second, checkpoint->getOrder());
    }
}

static void handleBridgeSensorTouch(BridgeSensor* sensor, Car* car) {
    if (sensor->getType() == RenderLayer::OVER) {
        car->setLayer(RenderLayer::OVER);
    } else {
        car->setLayer(RenderLayer::UNDER);
    }
}

void CollisionManager::processSensors(const b2SensorEvents& events) {
    for (int i = 0; i < events.beginCount; ++i) {
        const auto& ev = events.beginEvents[i];

        Entity* a = shapeToEntity.at(ev.sensorShapeId);
        Entity* b = shapeToEntity.at(ev.visitorShapeId);

        // Car ↔ Checkpoint
        if (isPair<Checkpoint, Car>(a, b))
            handleCheckpointTouch(a, b, raceSession, carToPlayer);
        else if (isPair<Car, Checkpoint>(a, b))
            handleCheckpointTouch(b, a, raceSession, carToPlayer);

        // Car ↔ BridgeSensor
        else if (isPair<BridgeSensor, Car>(a, b))
            handleBridgeSensorTouch(as<BridgeSensor>(a), as<Car>(b));

        else if (isPair<Car, BridgeSensor>(a, b))
            handleBridgeSensorTouch(as<BridgeSensor>(b), as<Car>(a));
    }
}

void CollisionManager::process(const b2ContactEvents& events) {
    handleHitEvents(events);
}
