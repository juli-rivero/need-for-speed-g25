//
// Created by elvis on 21/10/25.
//

#ifndef TALLER_TP_ENTITYFACTORY_H
#define TALLER_TP_ENTITYFACTORY_H
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
    static std::unique_ptr<Car> createCar(b2WorldId world,int playerId,const std::string color, float x, float y);
    static std::unique_ptr<Wall> createBuilding(b2WorldId world, float x, float y, float w, float h);
    static std::unique_ptr<Checkpoint> createCheckpoint(b2WorldId world, float x, float y, float radius,int order);
    static std::unique_ptr<Hint> createHint(float x, float y, float angle);
    static std::unique_ptr<Bridge> createBridge(b2WorldId world, float x, float y, float w, float h, bool driveable);
};

#endif
