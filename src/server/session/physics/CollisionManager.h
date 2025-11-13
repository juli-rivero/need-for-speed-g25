#pragma once

#include <box2d/box2d.h>

#include <iostream>
#include <memory>
#include <unordered_map>

#include "../logic/RaceSession.h"
#include "Box2DHash.h"

class CollisionManager {
   private:
    // Mapeo de cuerpo físico → puntero lógico (Car, Wall, Checkpoint, etc.)
    std::unordered_map<b2ShapeId, Entity*, b2ShapeIdHasher, b2ShapeIdEqual>
        shapeToEntity;
    std::unordered_map<const Car*,PlayerId> carToPlayer;
    RaceSession* raceSession{nullptr};

    void handleHitEvents(const b2ContactEvents& events);
    void resolvePhysicalImpact(void* a, void* b, float impact);
    //void handleBeginTouch(const b2ContactEvents& events);
    // void handleEndTouch (const b2ContactEvents& events);
   public:
    CollisionManager() = default;

    template <typename T>
    void registerEntity(b2ShapeId shapeId, T* entity) {
        static_assert(std::is_base_of_v<Entity, T>, "T debe derivar de Entity");
        shapeToEntity[shapeId] = entity;
    }
    void registerCar(const Car* car,PlayerId playerId) {
        carToPlayer[car] = playerId;
    }

    void unregisterEntity(b2ShapeId shapeId) { shapeToEntity.erase(shapeId); }
    void setRaceSession(RaceSession* session) { raceSession = session; }
    void processSensors(const b2SensorEvents& events);

    void process(const b2ContactEvents& events);
};
