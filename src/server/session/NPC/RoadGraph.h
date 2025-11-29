#pragma once
#include <box2d/box2d.h>

#include <vector>

#include "NPCType.h"

class RoadGraph {
   public:
    void addRoad(const RoadShape& road) { roads.push_back(road); }
    void build();

    const std::vector<IntersectionNode>& getNodes() const { return nodes; }

    const IntersectionNode* nearestNode(float x, float y) const;
    const std::vector<RoadSegment>& getSegments() const { return segments; }

   private:
    std::vector<RoadShape> roads;
    std::vector<IntersectionNode> nodes;
    std::vector<RoadSegment> segments;

    int nextNodeId = 0;

    bool isNear(const b2Vec2& a, const b2Vec2& b, float eps = 0.1f) const;
};
