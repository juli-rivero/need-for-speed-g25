#include <iostream>
#include <memory>
#include <vector>
#include <thread>
#include <chrono>
#include <cmath>

#include "model/Bridge.h"
#include "model/Wall.h"
#include "logic/types.h"
#include "config/MapLoader.h"
#include "config/YamlGameConfig.h"
#include "physics/Box2DPhysicsWorld.h"



int main() {
    try {
        std::cout << "=== TEST: Carga de configuración y mapa ===\n";

        // 1️⃣ Cargar configuración general
        YamlGameConfig cfg("./server/assets/config.yaml");

        // 2️⃣ Crear mundo físico (aunque no lo uses aún)
        Box2DPhysicsWorld world;

        // 3️⃣ Cargar un mapa (por ejemplo el primero del YAML)
        if (!cfg.getCities().empty()) {
            const auto& firstCity = cfg.getCities().front();
            if (!firstCity.races.empty()) {
                const auto& firstRace = firstCity.races.front();
                std::cout << "\nCargando mapa: " << firstRace.mapFile << "\n";

                std::vector<std::unique_ptr<Wall>> walls;
                std::vector<std::unique_ptr<Bridge>> bridges;
                std::vector<Checkpoint> checkpoints;
                std::vector<Hint> hints;
                std::vector<SpawnPoint> spawns;

                MapLoader::loadFromYAML(
                    firstRace.mapFile,
                    world,
                    walls,
                    bridges,
                    checkpoints,
                    hints,
                    spawns
                );

                std::cout << "\nResumen del mapa:\n";
                std::cout << "Walls: " << walls.size() << "\n";
                std::cout << "Bridges: " << bridges.size() << "\n";
                std::cout << " Checkpoints: " << checkpoints.size() << "\n";
                std::cout << "Hints: " << hints.size() << "\n";
                std::cout << "Spawns: " << spawns.size() << "\n";
            }
        }

        std::cout << "\nTodo cargado correctamente.\n";
    } catch (const std::exception& e) {
        std::cerr << " Error durante la carga: " << e.what() << "\n";
    }
}
