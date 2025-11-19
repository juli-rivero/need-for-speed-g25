//
// Created by elvis on 21/10/25.
//

#ifndef TALLER_TP_BRIDGE_H
#define TALLER_TP_BRIDGE_H
#include <memory>

#include "../physics/IPhysicalBody.h"



class Bridge {
private:
    int id;
    std::shared_ptr<IPhysicalBody> lowerBody;
    std::shared_ptr<IPhysicalBody> upperBody;

public:
    Bridge(int id, std::shared_ptr<IPhysicalBody> lower, std::shared_ptr<IPhysicalBody> upper)
        : id(id), lowerBody(std::move(lower)), upperBody(std::move(upper)) {}

    int getId() const { return id; }
    Vec2 getLowerPosition() const { return lowerBody->getPosition(); }
    Vec2 getUpperPosition() const { return upperBody->getPosition(); }
};
#endif //TALLER_TP_BRIDGE_H