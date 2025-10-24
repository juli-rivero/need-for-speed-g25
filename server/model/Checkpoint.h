#ifndef CHECKPOINT_H
#define CHECKPOINT_H

#include <cmath>
#include <memory>

#include "../physics/IPhysicalBody.h"

class Checkpoint {
private:
    int id;
    int order;
    float radius;
    std::shared_ptr<IPhysicalBody> body;

public:
    Checkpoint(int id, int order, float radius, std::shared_ptr<IPhysicalBody> body)
        : id(id), order(order), radius(radius), body(std::move(body)) {}

    int getId() const { return id; }
    int getOrder() const { return order; }

    Vec2 getPosition() const { return body->getPosition(); }
    float getAngle() const { return body->getAngle(); }

    bool isReached(const Vec2& carPos) const {
        Vec2 cp = getPosition();
        float dx = cp.x - carPos.x;
        float dy = cp.y - carPos.y;
        return std::sqrt(dx*dx + dy*dy) <= radius;
    }
};

#endif
