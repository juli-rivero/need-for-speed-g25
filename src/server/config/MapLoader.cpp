#include "MapLoader.h"
#include "../physics/EntityFactory.h"
#include "../physics/Box2DBodyAdapter.h"
#include <iostream>

void MapLoader::loadFromYAML(
    const std::string& path,
    PhysicsWorld& world,
    std::size_t raceIndex,
    std::vector<std::unique_ptr<Wall>>& walls,
    std::vector<std::unique_ptr<Bridge>>& bridges,
    std::vector<Checkpoint>& checkpoints,
    std::vector<Hint>& hints,
    std::vector<SpawnPoint>& spawnPoints,
    std::vector<PlayerConfig>& players
) {
    try {
        YAML::Node root = YAML::LoadFile(path);

        if (!root["map"]) {
            throw std::runtime_error("El archivo YAML no contiene el nodo 'map'");
        }

        const auto& mapNode = root["map"];
        auto worldId = world.getWorldId();

        // === Cargar gravedad del mapa ===
        if (mapNode["gravity"]) {
            auto g = mapNode["gravity"];
            b2World_SetGravity(worldId, {g[0].as<float>(), g[1].as<float>()});
        }

        // === Cargar paredes / edificios ===
        if (mapNode["walls"]) {
            for (const auto& w : mapNode["walls"]) {
                float x = w["x"].as<float>();
                float y = w["y"].as<float>();
                float width = w["w"].as<float>();
                float height = w["h"].as<float>();
                walls.push_back(EntityFactory::createBuilding(worldId, x, y, width, height));
            }
        }

        // === Cargar puentes ===
        if (mapNode["bridges"]) {
            for (const auto& b : mapNode["bridges"]) {
                float x = b["x"].as<float>();
                float y = b["y"].as<float>();
                float w = b["w"].as<float>();
                float h = b["h"].as<float>();
                bool driveable = b["driveable"].as<bool>(true);
                bridges.push_back(EntityFactory::createBridge(worldId, x, y, w, h, driveable));
            }
        }

        // === Cargar checkpoints ===
        if (mapNode["checkpoints"]) {
            int order = 0;
            for (const auto& cp : mapNode["checkpoints"]) {
                float x = cp["x"].as<float>();
                float y = cp["y"].as<float>();
                float radius = cp["radius"].as<float>();
                order = cp["order"] ? cp["order"].as<int>() : order + 1;

                auto c = EntityFactory::createCheckpoint(worldId, x, y, radius, order);
                checkpoints.push_back(*c); // copiar (o convertir a shared si preferís)
            }
        }

        // === Cargar hints (flechas de dirección) ===
        if (mapNode["hints"]) {
            for (const auto& h : mapNode["hints"]) {
                float x = h["x"].as<float>();
                float y = h["y"].as<float>();
                float angle = h["angle"].as<float>(0.0f);
                hints.push_back(*EntityFactory::createHint(x, y, angle));
            }
        }

        // === Cargar posiciones de spawn de jugadores ===
        if (mapNode["spawn_points"]) {
            for (const auto& sp : mapNode["spawn_points"]) {
                SpawnPoint point;
                point.x = sp["x"].as<float>();
                point.y = sp["y"].as<float>();
                point.angle = sp["angle"].as<float>(0.0f);
                spawnPoints.push_back(point);
            }
        }

        // === Cargar configuración de jugadores ===
        if (root["players"]) {
            for (const auto& p : root["players"]) {
                PlayerConfig cfg;
                cfg.id = p["id"].as<int>();
                cfg.color = p["color"].as<std::string>("red");
                players.push_back(cfg);
            }
        }

        std::cout << "Mapa " << path
                  << " cargado correctamente (race " << raceIndex << ")\n";

    } catch (const std::exception& ex) {
        std::cerr << " Error al cargar mapa desde YAML '" << path
                  << "': " << ex.what() << std::endl;
    }
}