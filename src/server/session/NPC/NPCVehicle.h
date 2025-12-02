#pragma once
#include <box2d/box2d.h>

#include <memory>

#include "NPCType.h"
#include "server/session/model/Car.h"

class NPCVehicle {
   public:
    explicit NPCVehicle(std::unique_ptr<Car>&& car);

    void setTargetNode(const IntersectionNode* node);
    void update(float dt);

    Car* getCar() const { return car.get(); }
    const IntersectionNode* getTarget() const { return targetNode; }
    const IntersectionNode* getPrevious() const { return previousNode; }
    NpcSnapshot buildSnapshot() const;

   private:
    std::unique_ptr<Car> car;
    const IntersectionNode* targetNode = nullptr;
    const IntersectionNode* previousNode = nullptr;
    b2Vec2 steeringSeek(const b2Vec2& target);
};
