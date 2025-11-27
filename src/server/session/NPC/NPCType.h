#pragma once

#include <vector>

#include "box2d/math_functions.h"

struct RoadShape {
    int id;
    float minX, maxX;
    float minY, maxY;
    b2Vec2 center;

    bool horizontal;
    bool vertical;
};
struct IntersectionNode {
    int id;
    b2Vec2 pos;
    std::vector<int> connectedRoadIds;
};

struct RoadSegment {
    int id;
    int roadId;
    const IntersectionNode* a;
    const IntersectionNode* b;

    b2Vec2 start;
    b2Vec2 end;

    bool horizontal;
};
