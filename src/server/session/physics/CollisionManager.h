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
    std::unordered_map<b2BodyId, Entity*, b2BodyIdHasher, b2BodyIdEqual>
        bodyToEntity;
    RaceSession* raceSession{nullptr};

    void handleHitEvents(const b2ContactEvents& events);
    void resolvePhysicalImpact(void* a, void* b, float impact);
    void handleBeginTouch(const b2ContactEvents& events);
    // void handleEndTouch (const b2ContactEvents& events);
   public:
    CollisionManager() = default;

    template <typename T>
    void registerEntity(b2BodyId bodyId, T* entity) {
        static_assert(std::is_base_of_v<Entity, T>, "T debe derivar de Entity");
        bodyToEntity[bodyId] = entity;
    }

    void unregisterEntity(b2BodyId bodyId) { bodyToEntity.erase(bodyId); }
    void setRaceSession(RaceSession* session) { raceSession = session; }

    void process(const b2ContactEvents& events);
};
