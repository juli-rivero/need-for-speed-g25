#ifndef WALL_H
#define WALL_H

#include <memory>
#include "../physics/IPhysicalBody.h"

class Wall {
private:
    int id;
    std::shared_ptr<IPhysicalBody> body;
public:
    Wall(int id, std::shared_ptr<IPhysicalBody> body)
        : id(id), body(std::move(body)) {}

    int getId() const { return id; }
    Vec2 getPosition() const { return body->getPosition(); }
    float getAngle() const { return body->getAngle(); }
};
#endif

