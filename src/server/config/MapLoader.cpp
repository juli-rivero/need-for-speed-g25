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
    std::vector<SpawnPoint>& spawnPoints, std::vector<RoadShape>& roadShapes,
    std::vector<std::unique_ptr<BridgeSensor>>& sensors) {
    spdlog::info("[MapLoader] Cargando mapa desde {}...", yamlPath);

    YAML::Node root;
    try {
        root = YAML::LoadFile(yamlPath);
    } catch (const std::exception& e) {
        spdlog::error("[MapLoader] Error cargando YAML: {}", e.what());
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

    spdlog::info("Mapa: {} | Ciudad: {} | Gravedad: [{}, {}]", info.name,
                 info.city, info.gravity.x, info.gravity.y);

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

            // Pasa a unidades físicas
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
                spdlog::warn(
                    "[MapLoader] Checkpoint inválido: faltan campos (x, y, "
                    "width, rotation)");
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
                spdlog::warn(
                    "[MapLoader] WARNING: checkpoint sin 'order', usando 0");
            }

            // Convierto a unidades físicas
            float x = mapToWorld(mx);
            float y = mapToWorld(my);
            float w = mapToWorld(mWidth);
            float h = mapToWorld(4.0f);  // altura fija 4 (en pixeles)

            // Crea cuerpo físico + modelo lógico
            auto cp =
                factory.createCheckpoint(x, y, w, h, rotDeg, order, cpType);
            checkpoints.push_back(std::move(cp));

            const auto& cpr = *checkpoints.back();
            spdlog::debug(
                "Checkpoint {} id={} type={} pos=({}, {}) size=({}, {}) "
                "angle={}",
                cpr.getOrder(), cpr.getId(), t, cpr.getPosition().x,
                cpr.getPosition().y, cpr.getWidth(), cpr.getHeight(),
                cpr.getAngle());
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
                const auto& vertsNode = s["vertices"];

                if (vertsNode.size() < 3) {
                    spdlog::warn("[MapLoader] Sensor con pocos vértices");
                    continue;
                }

                float minX = vertsNode[0]["x"].as<float>();
                float maxX = minX;
                float minY = vertsNode[0]["y"].as<float>();
                float maxY = minY;

                for (const auto& v : vertsNode) {
                    float vx = v["x"].as<float>();
                    float vy = v["y"].as<float>();
                    minX = std::min(minX, vx);
                    maxX = std::max(maxX, vx);
                    minY = std::min(minY, vy);
                    maxY = std::max(maxY, vy);
                }

                float cx = mapToWorld((minX + maxX) * 0.5f);
                float cy = mapToWorld((minY + maxY) * 0.5f);
                float w = mapToWorld(maxX - minX);
                float h = mapToWorld(maxY - minY);

                auto sensor = factory.createBridgeSensor(mode, cx, cy, w, h);
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
                spdlog::warn("[MapLoader] SpawnPoint inválido: faltan campos");
                continue;
            }

            float mx = n["x"].as<float>();
            float my = n["y"].as<float>();
            float angle = n["angle"].as<float>();

            float x = mapToWorld(mx);
            float y = mapToWorld(my);

            SpawnPoint sp(x, y, angle);
            spawnPoints.push_back(sp);

            spdlog::debug("SpawnPoint pos=({}, {}) angle={}", sp.x, sp.y,
                          sp.angle);
        }
    }

    if (mapNode["roads"]) {
        int idCounter = 0;
        for (const auto& r : mapNode["roads"]) {
            const auto& verts = r["vertices"];

            float minX = verts[0]["x"].as<float>();
            float maxX = minX;
            float minY = verts[0]["y"].as<float>();
            float maxY = minY;

            for (const auto& v : verts) {
                float x = v["x"].as<float>();
                float y = v["y"].as<float>();
                minX = std::min(minX, x);
                maxX = std::max(maxX, x);
                minY = std::min(minY, y);
                maxY = std::max(maxY, y);
            }

            RoadShape rs;
            rs.id = idCounter++;
            rs.minX = mapToWorld(minX);
            rs.maxX = mapToWorld(maxX);
            rs.minY = mapToWorld(minY);
            rs.maxY = mapToWorld(maxY);
            rs.center =
                b2Vec2((rs.minX + rs.maxX) / 2, (rs.minY + rs.maxY) / 2);
            rs.horizontal = ((rs.maxX - rs.minX) >= (rs.maxY - rs.minY));
            rs.vertical = !rs.horizontal;

            roadShapes.push_back(rs);
        }
    }

    spdlog::info(
        "Mapa cargado correctamente con {} walls, {} bridges, {} overpasses, "
        "{} checkpoints, {} sensors, {} spawn points.",
        buildings.size(), bridges.size(), overpasses.size(), checkpoints.size(),
        sensors.size(), spawnPoints.size());

    return info;
}
