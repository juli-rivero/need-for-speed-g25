#include "TrafficSystem.h"

#include <cstdlib>
#include <memory>
#include <utility>
#include <vector>

TrafficSystem::TrafficSystem(EntityFactory& f) : factory(f) {
    std::srand(std::time(nullptr));
}

const std::vector<std::unique_ptr<NPCVehicle>>& TrafficSystem::getNPCs() const {
    return npcs;
}
void TrafficSystem::spawnNPCs() {
    const auto& segments = graph->getSegments();
    if (segments.empty()) {
        return;
    }
    // se elige segmento
    const RoadSegment& seg = segments[rand() % segments.size()];

    // se interpola punto aleatorio sobre el segmento

    // NOLINTNEXTLINE
    float t = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
    b2Vec2 pos = {seg.start.x + (seg.end.x - seg.start.x) * t,
                  seg.start.y + (seg.end.y - seg.start.y) * t};

    float dx = seg.end.x - seg.start.x;
    float dy = seg.end.y - seg.start.y;
    float angle = std::atan2(dy, dx);

    // crea auto
    auto car = factory.createNpcCar(CarType::Classic, pos.x, pos.y);
    auto npc = std::make_unique<NPCVehicle>(std::move(car));
    npc->getCar()->setLayer(RenderLayer::UNDER);
    npc->getCar()->getBody()->setTransform(pos.x, pos.y, angle);

    npc->setTargetNode(seg.b);

    npcs.push_back(std::move(npc));
}

void TrafficSystem::update(float dt) const {
    const auto& segments = graph->getSegments();

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
