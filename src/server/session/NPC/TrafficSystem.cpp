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

    std::cout << "[NPC] Trying to spawn. Segments = " << segments.size()
              << std::endl;

    if (segments.empty()) {
        std::cout << "[NPC] ERROR: No segments in graph -> no roads detected!"
                  << std::endl;
        return;
    }

    // se elige segmento
    const RoadSegment& seg = segments[rand() % segments.size()];

    std::cout << "[NPC] Selected segment id=" << seg.id
              << " road=" << seg.roadId << " from (" << seg.start.x << ","
              << seg.start.y << ")" << " to (" << seg.end.x << "," << seg.end.y
              << ")" << std::endl;

    // se interpola punto aleatorio sobre el segmento

    // NOLINTNEXTLINE
    float t = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
    b2Vec2 pos = {seg.start.x + (seg.end.x - seg.start.x) * t,
                  seg.start.y + (seg.end.y - seg.start.y) * t};

    float dx = seg.end.x - seg.start.x;
    float dy = seg.end.y - seg.start.y;
    float angle = std::atan2(dy, dx);

    std::cout << "[NPC] Spawn at (" << pos.x << "," << pos.y
              << ") angle=" << angle << std::endl;

    // crea auto
    auto car = factory.createNpcCar(CarType::Classic, pos.x, pos.y);
    auto npc = std::make_unique<NPCVehicle>(std::move(car));
    npc->getCar()->setLayer(RenderLayer::UNDER);
    npc->getCar()->getBody()->setTransform(pos.x, pos.y, angle);

    // se asigna nodo target
    std::cout << "[NPC] Target = node "
              << (seg.b ? std::to_string(seg.b->id) : "NULL") << std::endl;

    npc->setTargetNode(seg.b);

    npcs.push_back(std::move(npc));
    std::cout << "[SPAWN] NPC spawned on segment " << seg.id
              << " → target node = " << (seg.b ? seg.b->id : -1) << "\n";
    std::cout << "[NPC] Total NPCs now = " << npcs.size() << std::endl;
}

void TrafficSystem::update(float dt) const {
    const auto& nodes = graph->getNodes();
    std::cout << "[NPC] Update frame: NPC count=" << npcs.size() << std::endl;
    for (auto& npc : npcs) {
        npc->update(dt);

        const auto* target = npc->getTarget();
        if (!target) continue;

        auto pos = npc->getCar()->getBody()->getPosition();
        float dx = target->pos.x - pos.x;
        float dy = target->pos.y - pos.y;
        float dist = std::sqrt(dx * dx + dy * dy);

        std::cout << "[NPC] npc id=" << npc->getCar()->getId() << " pos=("
                  << pos.x << "," << pos.y << ") ";

        if (dist < 0.8f) {
            std::cout << "[NPC] Reached node " << target->id << std::endl;

            const IntersectionNode* previous = npc->getPrevious();

            // caminos en este nodo (ids de roads)
            const auto& roadsHere = target->connectedRoadIds;

            //  Elegimos caminos que realmente lleven a algún nodo
            //    distinto de target y (si es posible) distinto de previous.
            std::vector<int> options;

            for (int rid : roadsHere) {
                bool hasValidNeighbor = false;

                for (const auto& n : nodes) {
                    if (n.id == target->id)
                        continue;  // no quedarse en el mismo

                    // si tenemos previous, evitamos que el ÚNICO vecino sea él
                    if (previous && n.id == previous->id) continue;

                    bool sharesRoad = false;
                    for (int nrid : n.connectedRoadIds) {
                        if (nrid == rid) {
                            sharesRoad = true;
                            break;
                        }
                    }
                    if (!sharesRoad) continue;

                    // hay al menos un nodo distinto alcanzable por este road
                    hasValidNeighbor = true;
                    break;
                }

                if (hasValidNeighbor) {
                    options.push_back(rid);
                }
            }

            bool allowBack = false;

            // Si no encontramos ninguna opción "sin volver", permitimos volver
            if (options.empty()) {
                options = roadsHere;
                allowBack = true;  // en este caso se permite previous
            }

            // Elegimos un road aleatorio de las opciones
            int nextRoad = options[std::rand() % options.size()];

            std::cout << "[NPC] At node " << target->id << " possible roads: ";
            for (auto r : target->connectedRoadIds) std::cout << r << " ";
            std::cout << " → NextRoad=" << nextRoad << "\n";

            //  Buscamos el nodo vecino más cercano sobre ese road
            const IntersectionNode* nearestNext = nullptr;
            float bestDist = 1e9f;

            for (const auto& n : nodes) {
                if (n.id == target->id) continue;

                // si NO estamos en modo "allowBack", evitamos explicitamente
                // previous
                if (!allowBack && previous && n.id == previous->id) {
                    continue;
                }

                bool sharesRoad = false;
                for (int rid : n.connectedRoadIds) {
                    if (rid == nextRoad) {
                        sharesRoad = true;
                        break;
                    }
                }
                if (!sharesRoad) continue;

                float dx2 = n.pos.x - target->pos.x;
                float dy2 = n.pos.y - target->pos.y;
                float d = dx2 * dx2 + dy2 * dy2;
                if (d < bestDist) {
                    bestDist = d;
                    nearestNext = &n;
                }
            }
            if (!nearestNext && previous && allowBack) {
                nearestNext = previous;
                std::cout << "[NPC] Fallback to previous node " << previous->id
                          << "\n";
            }
            npc->setTargetNode(nearestNext);
        }
    }
}

// para que los npcs puedan volver y dar vuelta en U
// void TrafficSystem::update(float dt) const {
//     const auto& nodes = graph->getNodes();
//     std::cout << "[NPC] Update frame: NPC count=" << npcs.size() <<
//     std::endl; for (auto& npc : npcs) {
//         npc->update(dt);
//
//         const auto* target = npc->getTarget();
//         if (!target) continue;
//
//         auto pos = npc->getCar()->getBody()->getPosition();
//         float dx = target->pos.x - pos.x;
//         float dy = target->pos.y - pos.y;
//         float dist = std::sqrt(dx * dx + dy * dy);
//
//         std::cout << "[NPC] npc id=" << npc->getCar()->getId() << " pos=("
//                   << pos.x << "," << pos.y << ") ";
//
//         if (dist < 0.8f) {
//             std::cout << "[NPC] Reached node " << target->id << std::endl;
//
//             const IntersectionNode* previous = npc->getPrevious();
//
//             // se obtiene la lista de caminos de este nodo
//             const auto& roadsHere = target->connectedRoadIds;
//
//             // Construimos lista de caminos que NO sean el camino de entrada
//             std::vector<int> options;
//
//             for (int rid : roadsHere) {
//                 if (!previous ||
//                     !std::count(previous->connectedRoadIds.begin(),
//                                 previous->connectedRoadIds.end(), rid)) {
//                     options.push_back(rid);
//                 }
//             }
//
//             // Si sólo había 1 opción , permite volver
//             if (options.empty()) {
//                 options = roadsHere;
//             }
//
//             // se elige un camino válido
//             int nextRoad = options[rand() % options.size()];
//             std::cout << "[NPC] At node " << target->id << " possible roads:
//             ";
//
//             for (auto r : target->connectedRoadIds) std::cout << r << " ";
//
//             std::cout << " → NextRoad=" << nextRoad << "\n";
//
//             // se busca el nodo siguiente válido sobre ese camino
//             const IntersectionNode* nearestNext = nullptr;
//             float bestDist = 1e9f;
//
//             for (const auto& n : nodes) {
//                 if (n.id == target->id) continue;
//
//                 bool sharesRoad = false;
//                 for (int rid : n.connectedRoadIds)
//                     if (rid == nextRoad) sharesRoad = true;
//
//                 if (sharesRoad) {
//                     float dx2 = n.pos.x - target->pos.x;
//                     float dy2 = n.pos.y - target->pos.y;
//                     float d = dx2 * dx2 + dy2 * dy2;
//                     if (d < bestDist) {
//                         bestDist = d;
//                         nearestNext = &n;
//                     }
//                 }
//             }
//
//             npc->setTargetNode(nearestNext);
//         }
//     }
// }
