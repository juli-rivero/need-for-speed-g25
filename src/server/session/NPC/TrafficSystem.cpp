#include "TrafficSystem.h"

#include <cstdlib>
#include <memory>
#include <utility>
#include <vector>

TrafficSystem::TrafficSystem(
    Box2DPhysicsWorld& world, const YamlGameConfig& cfg, const RoadGraph& graph,
    const std::vector<SpawnPointInfo>& playerSpawnPoints)
    : graph(graph), factory(world, cfg), playerSpawnPoints(playerSpawnPoints) {
    std::srand(std::time(nullptr));
}

static bool isNear(const Point& a, const SpawnPointInfo& b, float minDist) {
    float dx = a.x - b.pos.x;
    float dy = a.y - b.pos.y;
    return (dx * dx + dy * dy) < (minDist * minDist);
}

bool TrafficSystem::isTooCloseToSpawnPoints(const Point& pos) const {
    const float MIN_DIST = 10.0f;  // 6 metros → 60 px si PPM=10

    for (const auto& sp : playerSpawnPoints) {
        if (isNear(pos, sp, MIN_DIST)) return true;
    }
    return false;
}

const std::vector<std::unique_ptr<NPCVehicle>>& TrafficSystem::getNPCs() const {
    return npcs;
}
void TrafficSystem::spawnNPCs() {
    const int MAX_ATTEMPTS = 25;

    for (int attempt = 0; attempt < MAX_ATTEMPTS; attempt++) {
        const auto& segments = graph.getSegments();
        if (segments.empty()) return;

        const RoadSegment& seg = segments[rand() % segments.size()];

        // NOLINTNEXTLINE
        float t = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
        Point pos{seg.start.x + (seg.end.x - seg.start.x) * t,
                  seg.start.y + (seg.end.y - seg.start.y) * t};

        if (isTooCloseToSpawnPoints(pos)) continue;

        float dx = seg.end.x - seg.start.x;
        float dy = seg.end.y - seg.start.y;
        float angle = atan2f(dy, dx);

        auto car = factory.createNpcCar(CarType::Classic, pos);
        auto npc = std::make_unique<NPCVehicle>(std::move(car));

        npc->getCar()->setLayer(RenderLayer::UNDER);
        npc->getCar()->setTransform(pos, angle);

        npc->setTargetNode(seg.b);

        npcs.push_back(std::move(npc));
        return;  // spawn correcto
    }
}

void TrafficSystem::update(float dt) const {
    const auto& segments = graph.getSegments();

    for (auto& npc : npcs) {
        npc->update(dt);

        const IntersectionNode* target = npc->getTarget();
        if (!target) continue;

        auto pos = npc->getCar()->getBody()->getPosition();
        float dx = target->pos.x - pos.x;
        float dy = target->pos.y - pos.y;
        float dist = std::sqrt(dx * dx + dy * dy);

        // ¿Llegó al nodo?
        if (dist < 0.8f) {
            const IntersectionNode* previous = npc->getPrevious();
            std::vector<const IntersectionNode*> nextCandidates;

            // se determina vecinos REALES usando SECCIONES del grafo

            for (const auto& seg : segments) {
                // Si el target es "a", vecino = b
                if (seg.a->id == target->id) {
                    // Evitamos volver al anterior
                    if (!previous || seg.b->id != previous->id)
                        nextCandidates.push_back(seg.b);
                } else if (seg.b->id == target->id) {
                    if (!previous || seg.a->id != previous->id)
                        nextCandidates.push_back(seg.a);
                }
            }

            //  si no hay vecinos (calle sin salida), permitir volver

            if (nextCandidates.empty()) {
                for (const auto& seg : segments) {
                    if (seg.a->id == target->id)
                        nextCandidates.push_back(seg.b);
                    else if (seg.b->id == target->id)
                        nextCandidates.push_back(seg.a);
                }
            }

            // se elige un vecino válido aleatorio

            if (!nextCandidates.empty()) {
                const IntersectionNode* chosen =
                    nextCandidates[rand() % nextCandidates.size()];

                npc->setTargetNode(chosen);
            }
        }
    }
}
