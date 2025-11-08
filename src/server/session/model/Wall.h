#ifndef WALL_H
#define WALL_H

#include <memory>

#include "Entity.h"
#include "../physics/IPhysicalBody.h"

class Wall : public Entity{
private:
    float width;
    float height;
    std::shared_ptr<IPhysicalBody> body;
public:
    Wall(int id, float w,float h ,std::shared_ptr<IPhysicalBody> body)
        : Entity(id,EntityType::Wall),width(w),height(h), body(std::move(body)) {}

    int getId() const { return id; }
    Vec2 getPosition() const { return body->getPosition(); }
    float getAngle() const { return body->getAngle(); }
    float getWidth() const { return width; }
    float getHeight() const { return height; }

};
#endif

