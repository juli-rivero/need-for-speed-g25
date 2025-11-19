#ifndef NPCCAR_H
#define NPCCAR_H

#include "Car.h"
//TODO
class NpcCar : public Car {
public:
    NpcCar(int id, float x = 0, float y = 0, float angle = 0){
    }

    void update(float dt) override;
};

#endif
