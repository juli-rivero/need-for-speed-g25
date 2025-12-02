#include "MapLoader.h"

#include <algorithm>
#include <limits>

#include "YamlInclude.h"
#include "spdlog/spdlog.h"

// ============================================================
//  Convenciones de unidades
//  - El YAML está en "pixeles del mapa" (coinciden con el editor Qt)
//  - TODAS las entidades del mundo (walls, checkpoints, spawn_points,
//    bridges, overpasses, sensors) usan ya unidades físicas.
// ============================================================
MapLoader::MapLoader(const std::string& file, const YamlGameConfig& cfg)
    : root(loadYamlWithInclude(file)), convertor(cfg.getPixelsPerMeter()) {
    if (!root["map"]) {
        throw std::runtime_error(
            "[MapLoader] Falta la sección 'map' en el YAML del circuito.");
    }
    spdlog::debug("Map loaded from {}", file);
}
std::string MapLoader::getCityName() const {
    spdlog::debug("City name: {}", root["map"]["city"].as<std::string>());
    return root["map"]["city"].as<std::string>();
}
std::string MapLoader::getCircuitName() const {
    spdlog::debug("Circuit name: {}", root["map"]["name"].as<std::string>());
    return root["map"]["name"].as<std::string>();
}
std::vector<Bound> MapLoader::getWalls() const {
    std::vector<Bound> walls;
    for (const auto& n : root["map"]["buildings"])
        if (n["type"].as<std::string>() == "residential")
            walls.push_back(parseBound(n["vertices"]));
    spdlog::debug("Walls: {}", walls.size());
    return walls;
}
std::vector<Bound> MapLoader::getRailings() const {
    std::vector<Bound> railings;
    for (const auto& n : root["map"]["buildings"])
        if (n["type"].as<std::string>() == "railing")
            railings.push_back(parseBound(n["vertices"]));
    spdlog::debug("Railings: {}", railings.size());
    return railings;
}
std::vector<Bound> MapLoader::getBridges() const {
    std::vector<Bound> bridges;
    for (const auto& n : root["map"]["buildings"])
        if (n["type"].as<std::string>() == "bridge")
            bridges.push_back(parseBound(n["vertices"]));
    spdlog::debug("Bridges: {}", bridges.size());
    return bridges;
}
std::vector<Bound> MapLoader::getOverpasses() const {
    std::vector<Bound> overpasses;
    for (const auto& n : root["map"]["buildings"])
        if (n["type"].as<std::string>() == "overpass")
            overpasses.push_back(parseBound(n["vertices"]));
    spdlog::debug("Overpasses: {}", overpasses.size());
    return overpasses;
}
std::vector<SpawnPointInfo> MapLoader::getSpawnPoints() const {
    std::vector<SpawnPointInfo> spawns;
    for (const auto& n : root["map"]["spawn_points"]) {
        float x = n["x"].as<float>();
        float y = n["y"].as<float>();
        float angle = n["angle"].as<float>();

        x = convertor.toMeters(x);
        y = convertor.toMeters(y);
        angle = convertor.toRadians(angle);

        spawns.push_back(SpawnPointInfo{Point{x, y}, angle});
    }
    spdlog::debug("Spawn Points: {}", spawns.size());
    return spawns;
}
std::vector<CheckpointInfo> MapLoader::getCheckpoints() const {
    std::vector<CheckpointInfo> checkpoints;
    for (const auto& n : root["map"]["checkpoints"]) {
        float mx = n["x"].as<float>();
        float my = n["y"].as<float>();
        float mWidth = n["width"].as<float>();
        float rotDeg = n["rotation"].as<float>();
        uint32_t order = n["order"].as<uint32_t>();

        float x = convertor.toMeters(mx);
        float y = convertor.toMeters(my);
        float w = convertor.toMeters(mWidth);
        float h = convertor.toMeters(4.0f);  // altura fija 4 (en pixeles)
        rotDeg = convertor.toRadians(rotDeg);

        CheckpointType cpType = CheckpointType::Intermediate;
        std::string t = n["type"].as<std::string>();
        if (t == "start") {
            cpType = CheckpointType::Start;
        } else if (t == "finish") {
            cpType = CheckpointType::Finish;
        }

        Bound bound{Point{x, y}, w, h};
        checkpoints.push_back(CheckpointInfo{order, bound, rotDeg, cpType});
    }
    spdlog::debug("Checkpoints: {}", checkpoints.size());
    return checkpoints;
}
std::vector<Bound> MapLoader::getUpperSensors() const {
    std::vector<Bound> sensors;
    if (root["map"]["sensors"] && root["map"]["sensors"]["upper"]) {
        for (const auto& n : root["map"]["sensors"]["upper"])
            sensors.push_back(parseBound(n["vertices"]));
    }
    spdlog::debug("Upper Sensors: {}", sensors.size());
    return sensors;
}
std::vector<Bound> MapLoader::getLowerSensors() const {
    std::vector<Bound> sensors;
    if (root["map"]["sensors"] && root["map"]["sensors"]["lower"]) {
        for (const auto& n : root["map"]["sensors"]["lower"])
            sensors.push_back(parseBound(n["vertices"]));
    }
    spdlog::debug("Lower Sensors: {}", sensors.size());
    return sensors;
}
std::vector<RoadShape> MapLoader::getRoadShapes() const {
    std::vector<RoadShape> roadShapes;
    if (root["map"]["roads"]) {
        int idCounter = 0;
        for (const auto& r : root["map"]["roads"]) {
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

            minX = convertor.toMeters(minX);
            maxX = convertor.toMeters(maxX);
            minY = convertor.toMeters(minY);
            maxY = convertor.toMeters(maxY);
            const b2Vec2 center = {(minX + maxX) / 2, (minY + maxY) / 2};
            const bool horizontal = ((maxX - minX) >= (maxY - minY));

            roadShapes.push_back(RoadShape{
                .id = idCounter++,
                .minX = minX,
                .maxX = maxX,
                .minY = minY,
                .maxY = maxY,
                .center = center,
                .horizontal = horizontal,
                .vertical = !horizontal,
            });
        }
    }
    spdlog::debug("Road Shapes: {}", roadShapes.size());
    return roadShapes;
}
Bound MapLoader::parseBound(const YAML::Node& node) const {
    if (node.size() < 3)
        throw std::runtime_error(
            "Bound tiene que ser de por lo menos 3 vertices");
    // Bounding box en pixeles de mapa
    float minX = std::numeric_limits<float>::max();
    float maxX = std::numeric_limits<float>::lowest();
    float minY = std::numeric_limits<float>::max();
    float maxY = std::numeric_limits<float>::lowest();

    for (const auto& v : node) {
        float x = v["x"].as<float>();
        float y = v["y"].as<float>();
        minX = std::min(minX, x);
        maxX = std::max(maxX, x);
        minY = std::min(minY, y);
        maxY = std::max(maxY, y);
    }

    // Pasa a unidades físicas
    float w = convertor.toMeters(maxX - minX);
    float h = convertor.toMeters(maxY - minY);
    float cx = convertor.toMeters((minX + maxX) / 2);
    float cy = convertor.toMeters((minY + maxY) / 2);

    return {Point{cx, cy}, w, h};
}
