#pragma once

#include <memory>
#include <utility>

#include "../physics/IPhysicalBody.h"
#include "server/session/model/Entity.h"

class Bridge : public Entity {
   private:
    std::shared_ptr<IPhysicalBody> lowerBody;
    std::shared_ptr<IPhysicalBody> upperBody;

   public:
    Bridge(const int id, std::shared_ptr<IPhysicalBody> lower,
           std::shared_ptr<IPhysicalBody> upper)
        : Entity(id, EntityType::Bridge),
          lowerBody(std::move(lower)),
          upperBody(std::move(upper)) {}

    int getId() const { return id; }
    Vec2 getLowerPosition() const { return lowerBody->getPosition(); }
    Vec2 getUpperPosition() const { return upperBody->getPosition(); }
};
