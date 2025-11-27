#pragma once

#include <memory>
#include <utility>

#include "../physics/IPhysicalBody.h"
#include "Entity.h"

class Wall : public Entity {
   private:
    float width;
    float height;
    std::shared_ptr<IPhysicalBody> body;

   public:
    Wall(int id, float w, float h, std::shared_ptr<IPhysicalBody> body,
         EntityType type)
        : Entity(id, type), width(w), height(h), body(std::move(body)) {}

    Vec2 getPosition() const { return body->getPosition(); }
    float getAngle() const { return body->getAngle(); }
    float getWidth() const { return width; }
    float getHeight() const { return height; }
    static constexpr EntityType Type = EntityType::Wall;
};
