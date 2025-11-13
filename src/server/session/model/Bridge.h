#pragma once

#include <memory>
#include <utility>

#include "../physics/IPhysicalBody.h"
#include "server/session/model/Entity.h"

class Bridge : public Entity {
   private:
    std::shared_ptr<IPhysicalBody> lowerBody;
    std::shared_ptr<IPhysicalBody> upperBody;
    float width;
    float height;
    bool driveable;

   public:
    Bridge(int id, float w, float h, std::shared_ptr<IPhysicalBody> lower,
           std::shared_ptr<IPhysicalBody> upper, bool driveable = true)
        : Entity(id, EntityType::Bridge),
          lowerBody(std::move(lower)),
          upperBody(std::move(upper)),
          width(w),
          height(h),
          driveable(driveable) {}

    int getId() const override{ return id; }
    Vec2 getLowerPosition() const { return lowerBody->getPosition(); }
    Vec2 getUpperPosition() const { return upperBody->getPosition(); }
    float getWidth() const { return width; }
    float getHeight() const { return height; }

    bool isDriveable() const { return driveable; }
    void setDriveable(bool state) { driveable = state; }
};
