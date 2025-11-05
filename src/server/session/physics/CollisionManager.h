

#ifndef TALLER_TP_COLLISIONMANAGER_H
#define TALLER_TP_COLLISIONMANAGER_H


#include <box2d/box2d.h>
#include <unordered_map>
#include <memory>
#include <iostream>
#include "Box2DHash.h"
#include "../logic/RaceSession.h"

class CollisionManager {
private:
    // Mapeo de cuerpo físico → puntero lógico (Car, Wall, Checkpoint, etc.)
    std::unordered_map<b2BodyId, void*,b2BodyIdHasher,b2BodyIdEqual> bodyToEntity;
    RaceSession* raceSession{nullptr};

    void handleHitEvents(const b2ContactEvents& events);
    void resolvePhysicalImpact(void* a, void* b, float impact);
    void handleBeginTouch (const b2ContactEvents& events);
    //void handleEndTouch (const b2ContactEvents& events);
public:
    CollisionManager() = default;

    template <typename T>
    void registerEntity(b2BodyId bodyId, T* entity) {
        bodyToEntity[bodyId] = static_cast<void*>(entity);
    }

    void unregisterEntity(b2BodyId bodyId) {
        bodyToEntity.erase(bodyId);
    }
    void setRaceSession(RaceSession* session) { raceSession = session; }

    void process(const b2ContactEvents& events);
};


#endif //TALLER_TP_COLLISIONMANAGER_H