#ifndef CHECKPOINT_H
#define CHECKPOINT_H

#include <cmath>
#include <memory>

#include "Entity.h"
#include "../physics/IPhysicalBody.h"

class Checkpoint : public Entity{
private:

    int order;
    float width;
    float height;
    float angle;
    std::shared_ptr<IPhysicalBody> body;

public:
    Checkpoint(int id, int order, float width, float height, float angle,
               std::shared_ptr<IPhysicalBody> body)
        : Entity(id, EntityType::Checkpoint),
          order(order), width(width), height(height),
          angle(angle), body(std::move(body)) {}

    int getId() const { return id; }
    int getOrder() const { return order; }

    Vec2 getPosition() const { return body->getPosition(); }
    float getAngle() const { return angle; }
    float getWidth() const { return width; }
    float getHeight() const { return height; }

};

#endif
