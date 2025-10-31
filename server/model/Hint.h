#ifndef HINT_H
#define HINT_H

#include "../physics/IPhysicalBody.h"

class Hint {
private:
    int id;
    Vec2 position;
    float angle;

public:
    Hint(int id, float x, float y, float angle)
        : id(id), position({x, y}), angle(angle) {}

    int getId() const { return id; }
    Vec2 getPosition() const { return position; }
    float getAngle() const { return angle; }
};
#endif
