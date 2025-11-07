
#ifndef TALLER_TP_ENTITYFACTORY_H
#define TALLER_TP_ENTITYFACTORY_H
#include "Box2DPhysicsWorld.h"
#include "../model/Car.h"
#include "../model/Wall.h"
#include "../model/Checkpoint.h"
#include "../model/Hint.h"
#include "../model/Bridge.h"

class EntityFactory {
private:
    static int nextId() {
        static int counter = 0;
        return counter++;
    }
public:
    // Todos reciben una referencia al mundo físico
    static std::unique_ptr<Car> createCar(Box2DPhysicsWorld& world, const CarType& type, float x, float y);
    static std::unique_ptr<Wall> createWall(Box2DPhysicsWorld& world, float x, float y, float w, float h);
    static std::unique_ptr<Checkpoint> createCheckpoint(Box2DPhysicsWorld& world, float x, float y, float w,float h, float angle,int order);
    static std::unique_ptr<Hint> createHint(float x, float y, float angle);
    static std::unique_ptr<Bridge> createBridge(Box2DPhysicsWorld& world, float x, float y, float w, float h, bool driveable);
};

#endif
