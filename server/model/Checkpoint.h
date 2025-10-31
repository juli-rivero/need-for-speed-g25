#ifndef CHECKPOINT_H
#define CHECKPOINT_H

#include <cmath>
#include <memory>

#include "../physics/IPhysicalBody.h"

class Checkpoint {
private:
    int id;
    int order;
    float width;
    float height;

    std::shared_ptr<IPhysicalBody> body;

public:
    Checkpoint(int id, int order, float width, float height, std::shared_ptr<IPhysicalBody> body)
        : id(id), order(order), width(width), height(height), body(std::move(body)) {}

    int getId() const { return id; }
    int getOrder() const { return order; }

    Vec2 getPosition() const { return body->getPosition(); }

    float getWidth() const { return width; }
    float getHeight() const { return height; }
};

#endif
