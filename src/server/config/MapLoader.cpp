#include "MapLoader.h"

#include <algorithm>
#include <iostream>
#include <utility>

#include "../session/model/BridgeSensor.h"
#include "../session/physics/Box2DBodyAdapter.h"
#include "../session/physics/EntityFactory.h"

static constexpr float MAP_SCALE = 0.1f;

MapLoader::MapInfo MapLoader::loadFromYAML(
    const std::string& yamlPath, EntityFactory& factory,
    std::vector<std::unique_ptr<Wall>>& buildings,
    std::vector<BridgeInfo>& bridges, std::vector<OverpassInfo>& overpasses,
    std::vector<std::unique_ptr<Checkpoint>>& checkpoints,
    std::vector<SpawnPoint>& spawnPoints,
    std::vector<std::unique_ptr<BridgeSensor>>& sensors

) {
    std::cout << "[MapLoader] Cargando mapa desde " << yamlPath << "...\n";
    YAML::Node root;

    try {
        root = YAML::LoadFile(yamlPath);
    } catch (const std::exception& e) {
        std::cerr << "Error cargando YAML: " << e.what() << std::endl;
        throw;
    }

    if (!root["map"]) {
        throw std::runtime_error(
            "Falta la sección 'map' en el YAML del circuito.");
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

    // 1) BUILDINGS
    if (mapNode["buildings"]) {
        for (const auto& n : mapNode["buildings"]) {
            const auto& verts = n["vertices"];
            std::vector<b2Vec2> poly;
            for (const auto& v : verts) {
                poly.push_back({v["x"].as<float>(), v["y"].as<float>()});
            }

            float minX = poly[0].x;
            float maxX = minX;
            float minY = poly[0].y;
            float maxY = minY;

            for (const auto& p : poly) {
                minX = std::min(minX, p.x);
                maxX = std::max(maxX, p.x);
                minY = std::min(minY, p.y);
                maxY = std::max(maxY, p.y);
            }

            float w = (maxX - minX) * MAP_SCALE;
            float h = (maxY - minY) * MAP_SCALE;
            float cx = (minX + maxX) * 0.5f * MAP_SCALE;
            float cy = (minY + maxY) * 0.5f * MAP_SCALE;

            std::string btype =
                n["type"] ? n["type"].as<std::string>() : "residential";

            if (btype == "bridge") {
                BridgeInfo bi{cx, cy, w, h};
                bridges.push_back(bi);
                continue;
            }

            if (btype == "overpass") {
                OverpassInfo oi{cx, cy, w, h};
                overpasses.push_back(oi);
                continue;
            }

            EntityType et = EntityType::Wall;
            if (btype == "railing") et = EntityType::Railing;

            auto wall = factory.createBuilding(cx, cy, w, h, et);
            buildings.push_back(std::move(wall));
        }
    }

    if (mapNode["checkpoints"]) {
        for (const auto& n : mapNode["checkpoints"]) {
            auto cp = factory.createCheckpoint(
                n["x"].as<float>(), n["y"].as<float>(), n["w"].as<float>(),
                n["h"].as<float>(), n["angle"].as<float>(),
                n["order"].as<int>());

            checkpoints.push_back(std::move(cp));
            const auto& cpr = *checkpoints.back();
            std::cout << "Checkpoint " << cpr.getOrder()
                      << " id=" << cpr.getId() << " pos=("
                      << cpr.getPosition().x << "," << cpr.getPosition().y
                      << ") size=(" << cpr.getWidth() << "," << cpr.getHeight()
                      << ")\n";
        }
    }
    // 4) BRIDGE SENSORS
    if (mapNode["sensors"]) {
        const auto& sn = mapNode["sensors"];

        auto mkList = [&](const YAML::Node& arr, BridgeSensorType mode) {
            for (const auto& s : arr) {
                float x = s["x"].as<float>() * MAP_SCALE;
                float y = s["y"].as<float>() * MAP_SCALE;
                float w = s["w"].as<float>() * MAP_SCALE;
                float h = s["h"].as<float>() * MAP_SCALE;

                auto sensor = factory.createBridgeSensor(mode, x, y, w, h);
                sensors.push_back(std::move(sensor));
            }
        };

        if (sn["upper"] && sn["upper"].IsSequence()) {
            mkList(sn["upper"], BridgeSensorType::SetUpper);
        }
        if (sn["lower"] && sn["lower"].IsSequence()) {
            mkList(sn["lower"], BridgeSensorType::SetLower);
        }
    }
    if (mapNode["spawn_points"]) {
        for (const auto& n : mapNode["spawn_points"]) {
            SpawnPoint sp(n["x"].as<float>(), n["y"].as<float>(),
                          n["angle"].as<float>());
            spawnPoints.push_back(sp);
            std::cout << "SpawnPoint pos=(" << sp.x << "," << sp.y
                      << ") angle=" << sp.angle << "\n";
        }
    }

    std::cout << "Mapa cargado correctamente con " << buildings.size()
              << " walls, " << bridges.size() << " bridges, "
              << checkpoints.size() << spawnPoints.size() << " spawn points.\n";

    return info;
}
