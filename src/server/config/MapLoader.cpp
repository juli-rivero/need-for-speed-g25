#include "MapLoader.h"

#include <algorithm>
#include <iostream>
#include <utility>

#include "../session/physics/Box2DBodyAdapter.h"
#include "../session/physics/EntityFactory.h"
static constexpr float MAP_SCALE = 0.1f;

MapLoader::MapInfo MapLoader::loadFromYAML(
    const std::string& yamlPath, Box2DPhysicsWorld& world,
    std::vector<std::unique_ptr<Wall>>& walls,
    std::vector<std::unique_ptr<Bridge>>& bridges,
    std::vector<std::unique_ptr<Checkpoint>>& checkpoints,
    std::vector<SpawnPoint>& spawnPoints

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

    EntityFactory factory;

    if (mapNode["walls"]) {
        for (const auto& n : mapNode["walls"]) {
            const auto& verts = n["vertices"];
            std::vector<b2Vec2> poly;

            for (const auto& v : verts) {
                poly.push_back({v["x"].as<float>(), v["y"].as<float>()});
            }

            // calcular bounding box
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

            auto wall = factory.createWall(world, cx, cy, w, h);
            walls.push_back(std::move(wall));
        }
    }

    if (mapNode["bridges"]) {
        for (const auto& n : mapNode["bridges"]) {
            auto id = n["id"].as<int>();
            float x = n["x"].as<float>();
            float y = n["y"].as<float>();
            float w = n["w"].as<float>();
            float h = n["h"].as<float>();
            bool driveable = n["driveable"] ? n["driveable"].as<bool>() : true;

            auto bridge = factory.createBridge(world, x, y, w, h, driveable);
            bridges.push_back(std::move(bridge));

            std::cout << " Bridge id=" << id << " driveable=" << driveable
                      << " pos=(" << x << "," << y << ")\n";
        }
    }

    if (mapNode["checkpoints"]) {
        for (const auto& n : mapNode["checkpoints"]) {
            auto cp = factory.createCheckpoint(
                world, n["x"].as<float>(), n["y"].as<float>(),
                n["w"].as<float>(), n["h"].as<float>(), n["angle"].as<float>(),
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

    if (mapNode["spawn_points"]) {
        for (const auto& n : mapNode["spawn_points"]) {
            SpawnPoint sp(n["x"].as<float>(), n["y"].as<float>(),
                          n["angle"].as<float>());
            spawnPoints.push_back(sp);
            std::cout << "SpawnPoint pos=(" << sp.x << "," << sp.y
                      << ") angle=" << sp.angle << "\n";
        }
    }

    std::cout << "Mapa cargado correctamente con " << walls.size() << " walls, "
              << bridges.size() << " bridges, " << checkpoints.size()
              << spawnPoints.size() << " spawn points.\n";

    return info;
}
