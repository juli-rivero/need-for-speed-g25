#include "MapLoader.h"

#include <algorithm>
#include <iostream>
#include <limits>
#include <utility>

#include "../session/model/BridgeSensor.h"
#include "../session/physics/Box2DBodyAdapter.h"
#include "../session/physics/EntityFactory.h"

// ============================================================
//  Convenciones de unidades
//  - El YAML está en "pixeles del mapa" (coinciden con el editor Qt)
//  - MAP_SCALE convierte esos pixeles a "unidades físicas" (metros Box2D)
//  - TODAS las entidades del mundo (walls, checkpoints, spawn_points,
//    bridges, overpasses, sensors) usan ya unidades físicas.
// ============================================================
static constexpr float MAP_SCALE = 0.1f;  // 1 px de mapa = 0.1 unidades físicas

static inline float mapToWorld(float v) { return v * MAP_SCALE; }

MapLoader::MapInfo MapLoader::loadFromYAML(
    const std::string& yamlPath, EntityFactory& factory,
    std::vector<std::unique_ptr<Wall>>& buildings, std::vector<Bound>& bridges,
    std::vector<Bound>& overpasses,
    std::vector<std::unique_ptr<Checkpoint>>& checkpoints,
    std::vector<SpawnPoint>& spawnPoints,
    std::vector<std::unique_ptr<BridgeSensor>>& sensors) {
    std::cout << "[MapLoader] Cargando mapa desde " << yamlPath << "...\n";

    YAML::Node root;
    try {
        root = YAML::LoadFile(yamlPath);
    } catch (const std::exception& e) {
        std::cerr << "[MapLoader] Error cargando YAML: " << e.what() << "\n";
        throw;
    }

    if (!root["map"]) {
        throw std::runtime_error(
            "[MapLoader] Falta la sección 'map' en el YAML del circuito.");
    }

    const auto& mapNode = root["map"];

    MapInfo info;
    info.name =
        mapNode["name"] ? mapNode["name"].as<std::string>() : "Unnamed Map";
    info.city = mapNode["city"] ? mapNode["city"].as<std::string>() : "Unknown";

    if (mapNode["gravity"] && mapNode["gravity"].IsSequence() &&
        mapNode["gravity"].size() == 2) {
        info.gravity = {mapNode["gravity"][0].as<float>(),
                        mapNode["gravity"][1].as<float>()};
    } else {
        info.gravity = {0.0f, 0.0f};
    }

    std::cout << "Mapa: " << info.name << " | Ciudad: " << info.city
              << " | Gravedad: [" << info.gravity.x << ", " << info.gravity.y
              << "]\n";

    // ============================================================
    // 1) BUILDINGS / WALLS / BRIDGES / OVERPASSES
    // ============================================================
    if (mapNode["buildings"]) {
        for (const auto& n : mapNode["buildings"]) {
            if (!n["vertices"] || !n["vertices"].IsSequence()) {
                continue;
            }

            const auto& vertsNode = n["vertices"];
            if (vertsNode.size() < 3) {
                continue;
            }

            // Leer vértices en coordenadas de mapa (pixeles)
            std::vector<b2Vec2> poly;
            poly.reserve(vertsNode.size());
            for (const auto& v : vertsNode) {
                float mx = v["x"].as<float>();
                float my = v["y"].as<float>();
                poly.push_back({mx, my});
            }

            // Bounding box en pixeles de mapa
            float minX = std::numeric_limits<float>::max();
            float maxX = std::numeric_limits<float>::lowest();
            float minY = std::numeric_limits<float>::max();
            float maxY = std::numeric_limits<float>::lowest();

            for (const auto& p : poly) {
                minX = std::min(minX, p.x);
                maxX = std::max(maxX, p.x);
                minY = std::min(minY, p.y);
                maxY = std::max(maxY, p.y);
            }

            // Pasar a unidades físicas
            float w = mapToWorld(maxX - minX);
            float h = mapToWorld(maxY - minY);
            float cx = mapToWorld((minX + maxX) * 0.5f);
            float cy = mapToWorld((minY + maxY) * 0.5f);

            std::string btype =
                n["type"] ? n["type"].as<std::string>() : "residential";

            Bound bound{Point{cx, cy}, w, h};

            // Estos rectángulos especiales no son paredes físicas
            if (btype == "bridge") {
                bridges.push_back(bound);
                continue;
            }
            if (btype == "overpass") {
                overpasses.push_back(bound);
                continue;
            }

            // El resto son "buildings" físicos → walls
            EntityType et = EntityType::Wall;
            if (btype == "railing") {
                et = EntityType::Railing;
            }

            auto wall = factory.createBuilding(cx, cy, w, h, et);
            buildings.push_back(std::move(wall));
        }
    }

    // ============================================================
    // 2) CHECKPOINTS
    //    YAML nuevo:
    //    - type: start/intermediate/finish
    //      x, y, rotation, width, order
    // ============================================================
    if (mapNode["checkpoints"]) {
        for (const auto& n : mapNode["checkpoints"]) {
            if (!n["x"] || !n["y"] || !n["width"] || !n["rotation"]) {
                std::cerr << "[MapLoader] Checkpoint inválido: faltan campos "
                          << "(x, y, width, rotation)\n";
                continue;
            }

            std::string t = n["type"].as<std::string>();
            CheckpointType cpType = (t == "start") ? CheckpointType::Start
                                    : (t == "finish")
                                        ? CheckpointType::Finish
                                        : CheckpointType::Intermediate;

            float mx = n["x"].as<float>();
            float my = n["y"].as<float>();
            float mWidth = n["width"].as<float>();
            float rotDeg = n["rotation"].as<float>();

            // order es importante para el progreso de la carrera
            int order = 0;
            if (n["order"]) {
                order = n["order"].as<int>();
            } else {
                std::cerr << "[MapLoader] WARNING: checkpoint sin 'order', "
                             "usando 0\n";
            }

            // Convertir a unidades físicas
            float x = mapToWorld(mx);
            float y = mapToWorld(my);
            float w = mapToWorld(mWidth);
            float h =
                mapToWorld(4.0f);  // altura fija 4 (en pixeles) → escalate

            // Crear cuerpo físico + modelo lógico
            auto cp =
                factory.createCheckpoint(x, y, w, h, rotDeg, order, cpType);
            checkpoints.push_back(std::move(cp));

            const auto& cpr = *checkpoints.back();
            std::cout << "Checkpoint " << cpr.getOrder()
                      << " id=" << cpr.getId() << " type=" << t << " pos=("
                      << cpr.getPosition().x << "," << cpr.getPosition().y
                      << ")" << " size=(" << cpr.getWidth() << ","
                      << cpr.getHeight() << ")" << " angle=" << cpr.getAngle()
                      << "\n";
        }
    }

    // ============================================================
    // 3) BRIDGE SENSORS
    //    map.sensors.upper / lower (x,y,w,h) en pixeles de mapa
    // ============================================================
    if (mapNode["sensors"]) {
        const auto& sn = mapNode["sensors"];

        auto mkList = [&](const YAML::Node& arr, RenderLayer mode) {
            for (const auto& s : arr) {
                if (!s["x"] || !s["y"] || !s["w"] || !s["h"]) {
                    std::cerr << "[MapLoader] Sensor inválido: faltan campos\n";
                    continue;
                }

                float mx = s["x"].as<float>();
                float my = s["y"].as<float>();
                float mw = s["w"].as<float>();
                float mh = s["h"].as<float>();

                float x = mapToWorld(mx);
                float y = mapToWorld(my);
                float w = mapToWorld(mw);
                float h = mapToWorld(mh);

                auto sensor = factory.createBridgeSensor(mode, x, y, w, h);
                sensors.push_back(std::move(sensor));
            }
        };

        if (sn["upper"] && sn["upper"].IsSequence()) {
            mkList(sn["upper"], RenderLayer::OVER);
        }
        if (sn["lower"] && sn["lower"].IsSequence()) {
            mkList(sn["lower"], RenderLayer::UNDER);
        }
    }

    // ============================================================
    // 4) SPAWN POINTS
    //    map.spawn_points: x,y,angle en pixeles de mapa
    // ============================================================
    if (mapNode["spawn_points"]) {
        for (const auto& n : mapNode["spawn_points"]) {
            if (!n["x"] || !n["y"] || !n["angle"]) {
                std::cerr << "[MapLoader] SpawnPoint inválido: faltan campos\n";
                continue;
            }

            float mx = n["x"].as<float>();
            float my = n["y"].as<float>();
            float angle = n["angle"].as<float>();

            float x = mapToWorld(mx);
            float y = mapToWorld(my);

            SpawnPoint sp(x, y, angle);
            spawnPoints.push_back(sp);

            std::cout << "SpawnPoint pos=(" << sp.x << "," << sp.y
                      << ") angle=" << sp.angle << "\n";
        }
    }

    std::cout << "Mapa cargado correctamente con " << buildings.size()
              << " walls, " << bridges.size() << " bridges, "
              << overpasses.size() << " overpasses, " << checkpoints.size()
              << " checkpoints, " << sensors.size() << " sensors, "
              << spawnPoints.size() << " spawn points.\n";

    return info;
}
