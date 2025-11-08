#include "MapLoader.h"

#include <iostream>
#include <utility>

#include "../session/physics/Box2DBodyAdapter.h"
#include "../session/physics/EntityFactory.h"

MapLoader::MapInfo MapLoader::loadFromYAML(
    const std::string& yamlPath, Box2DPhysicsWorld& world,
    std::vector<std::unique_ptr<Wall>>& walls,
    std::vector<std::unique_ptr<Bridge>>& bridges,
    std::vector<Checkpoint>& checkpoints, std::vector<Hint>& hints,
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
            auto id = n["id"].as<int>();
            float x = n["x"].as<float>();
            float y = n["y"].as<float>();
            float w = n["w"].as<float>();
            float h = n["h"].as<float>();

            auto wall = factory.createWall(world, x, y, w, h);
            walls.push_back(std::move(wall));

            std::cout << "Wall id=" << id << " pos=(" << x << "," << y
                      << ") size=(" << w << "," << h << ")\n";
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

            checkpoints.push_back(*cp);
            std::cout << "Checkpoint " << cp->getOrder()
                      << " id=" << cp->getId() << " pos=("
                      << cp->getPosition().x << "," << cp->getPosition().y
                      << ") size=(" << cp->getWidth() << "," << cp->getHeight()
                      << "\n";
        }
    }

    if (mapNode["hints"]) {
        for (const auto& n : mapNode["hints"]) {
            Hint h(n["id"].as<int>(), n["x"].as<float>(), n["y"].as<float>(),
                   n["angle"].as<float>());

            hints.push_back(h);

            std::cout << "Hint id=" << h.getId() << " pos=("
                      << h.getPosition().x << "," << h.getPosition().y
                      << ") angle=" << h.getAngle() << "\n";
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
              << " checkpoints, " << hints.size() << " hints, "
              << spawnPoints.size() << " spawn points.\n";

    return info;
}
