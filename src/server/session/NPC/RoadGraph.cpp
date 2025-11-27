#include "RoadGraph.h"

#include <algorithm>
#include <cmath>
#include <iostream>

#include "NPCType.h"

bool RoadGraph::isNear(const b2Vec2& a, const b2Vec2& b, float eps) const {
    return (std::abs(a.x - b.x) <= eps && std::abs(a.y - b.y) <= eps);
}

// al buildear se crean nodos con las interseccion y segmentos
void RoadGraph::build() {
    for (size_t i = 0; i < roads.size(); ++i) {
        for (size_t j = i + 1; j < roads.size(); ++j) {
            const auto& A = roads[i];
            const auto& B = roads[j];

            bool overlapX = (A.minX <= B.maxX) && (A.maxX >= B.minX);
            bool overlapY = (A.minY <= B.maxY) && (A.maxY >= B.minY);

            bool perpendicular = A.horizontal != B.horizontal;

            if (overlapX && overlapY && perpendicular) {
                float ix1 = std::max(A.minX, B.minX);
                float ix2 = std::min(A.maxX, B.maxX);
                float iy1 = std::max(A.minY, B.minY);
                float iy2 = std::min(A.maxY, B.maxY);

                float cx = (ix1 + ix2) * 0.5f;
                float cy = (iy1 + iy2) * 0.5f;

                b2Vec2 pos(cx, cy);

                bool merged = false;
                for (auto& node : nodes) {
                    if (isNear(node.pos, pos, 0.5)) {
                        node.connectedRoadIds.push_back(A.id);
                        node.connectedRoadIds.push_back(B.id);
                        merged = true;
                        break;
                    }
                }

                if (!merged) {
                    IntersectionNode node;
                    node.id = nextNodeId++;
                    node.pos = pos;
                    node.connectedRoadIds = {A.id, B.id};
                    nodes.push_back(node);
                }
            }
        }
    }
    for (auto& node : nodes) {
        std::sort(node.connectedRoadIds.begin(), node.connectedRoadIds.end());
        node.connectedRoadIds.erase(std::unique(node.connectedRoadIds.begin(),
                                                node.connectedRoadIds.end()),
                                    node.connectedRoadIds.end());
    }

    segments.clear();
    int segId = 0;

    for (const auto& road : roads) {
        // busca intersecciones de este road
        std::vector<const IntersectionNode*> roadNodes;

        for (const auto& node : nodes) {
            for (int rid : node.connectedRoadIds) {
                if (rid == road.id) {
                    roadNodes.push_back(&node);
                    break;
                }
            }
        }

        if (roadNodes.size() < 2) continue;

        // ordena por posición en el eje dominante
        std::sort(roadNodes.begin(), roadNodes.end(),
                  [&](const IntersectionNode* a, const IntersectionNode* b) {
                      if (road.horizontal)
                          return a->pos.x < b->pos.x;
                      else
                          return a->pos.y < b->pos.y;
                  });

        // se crea segmentos consecutivos
        for (size_t i = 0; i + 1 < roadNodes.size(); ++i) {
            RoadSegment seg;
            seg.id = segId++;
            seg.roadId = road.id;
            seg.a = roadNodes[i];
            seg.b = roadNodes[i + 1];
            seg.start = seg.a->pos;
            seg.end = seg.b->pos;
            seg.horizontal = road.horizontal;
            segments.push_back(seg);
        }
    }
}

const IntersectionNode* RoadGraph::nearestNode(float x, float y) const {
    if (nodes.empty()) return nullptr;

    float bestDist = 999999.f;
    const IntersectionNode* best = nullptr;

    for (const auto& n : nodes) {
        float dx = n.pos.x - x;
        float dy = n.pos.y - y;
        float d = dx * dx + dy * dy;
        if (d < bestDist) {
            bestDist = d;
            best = &n;
        }
    }
    return best;
}
