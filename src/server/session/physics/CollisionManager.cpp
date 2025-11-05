#include "CollisionManager.h"
#include "../model/Car.h"
#include "../model/Checkpoint.h"
#include "../model/Wall.h"
#include <cmath>

#include "../model/Hint.h"
class Hint;

void CollisionManager::handleHitEvents(const b2ContactEvents& events) {
    for (int i = 0; i < events.hitCount; ++i) {
        const b2ContactHitEvent& ev = events.hitEvents[i];
        b2BodyId bodyA = b2Shape_GetBody(ev.shapeIdA);
        b2BodyId bodyB = b2Shape_GetBody(ev.shapeIdB);
        float impact = ev.approachSpeed;

        auto itA = bodyToEntity.find(bodyA);
        auto itB = bodyToEntity.find(bodyB);
        if (itA == bodyToEntity.end() || itB == bodyToEntity.end()) continue;

        void* entA = itA->second;
        void* entB = itB->second;

        resolvePhysicalImpact(entA, entB, impact);
    }
}
void CollisionManager::resolvePhysicalImpact(void* a, void* b, float impact) {
    Car* carA = dynamic_cast<Car*>(static_cast<Car *>(a));
    Car* carB = dynamic_cast<Car*>(static_cast<Car *>(b));
    Wall* wallA = dynamic_cast<Wall*>(static_cast<Wall *>(a));
    Wall* wallB = dynamic_cast<Wall*>(static_cast<Wall *>(b));

    if (carA && carB) {
        carA->damage(impact * 0.5f);
        carB->damage(impact * 0.5f);
        return;
    }

    if (carA && (wallB || !carB)) {
        carA->damage(impact * 0.7f);
        return;
    }

    if (carB && (wallA || !carA)) {
        carB->damage(impact * 0.7f);
        return;
    }
}
void CollisionManager::handleBeginTouch(const b2ContactEvents& events) {
    for (int i = 0; i < events.beginCount; ++i) {
        const b2ContactBeginTouchEvent& ev = events.beginEvents[i];
        b2BodyId bodyA = b2Shape_GetBody(ev.shapeIdA);
        b2BodyId bodyB = b2Shape_GetBody(ev.shapeIdB);

        auto itA = bodyToEntity.find(bodyA);
        auto itB = bodyToEntity.find(bodyB);
        if (itA == bodyToEntity.end() || itB == bodyToEntity.end()) continue;

        Car* car = nullptr;
        Checkpoint* cp = nullptr;
        Hint* hint = nullptr;

        // reconoce tipos
        if (auto c = dynamic_cast<Car*>((Car*)itA->second)) car = c;
        if (auto c = dynamic_cast<Car*>((Car*)itB->second)) car = c;
        if (auto ch = dynamic_cast<Checkpoint*>((Checkpoint*)itA->second)) cp = ch;
        if (auto ch = dynamic_cast<Checkpoint*>((Checkpoint*)itB->second)) cp = ch;
        if (auto h = dynamic_cast<Hint*>((Hint*)itA->second)) hint = h;
        if (auto h = dynamic_cast<Hint*>((Hint*)itB->second)) hint = h;

        if (car && cp)
            std::cout << " Car<" << car->getId() << "> reached checkpoint " << cp->getOrder() << std::endl;

        if (car && hint)
            std::cout << " Car<" << car->getId() << "> passed hint" << std::endl;
    }
}
/*void CollisionManager::handleEndTouch(const b2ContactEvents& events) {
    for (int i = 0; i < events.endCount; ++i) {
        const b2ContactEndTouchEvent& ev = events.endEvents[i];
        // similar a begin, pero para salida
    }
}*/
void CollisionManager::process(const b2ContactEvents& events) {
    handleHitEvents(events);
    handleBeginTouch(events);
    //handleEndTouch(events);
}
