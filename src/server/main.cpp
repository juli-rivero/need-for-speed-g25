#include <iostream>
#include <memory>
#include <vector>
#include <thread>
#include <chrono>
#include <cmath>

#include "session/model/Bridge.h"
#include "session/model/Wall.h"
#include "session/logic/types.h"
#include "config/MapLoader.h"
#include "config/YamlGameConfig.h"
#include "session/logic/GameSessionFacade.h"
#include "session/physics/Box2DPhysicsWorld.h"
#include "session/physics/EntityFactory.h"


// ============================================================
// MAIN DE PRUEBAS MANUALES (FISICAS,LOGICA,SERVIDOR)
// ============================================================
//
// Pruebas independientes del backend.
// Cada bloque se puede activar/desactivar para testear partes
// específicas del sistema.
//
// Secciones:
//   1 Carga de configuración global (YAML)
//   2 Carga de mapa y entidades físicas
//   3 Creación de autos y configuración del Match
//   4 Simulación básica con movimiento
//   5 Snapshot dinámico (para cliente)
//   6 Test de múltiples jugadores
//
// ============================================================
int main() {
    try {
        // ============================================================
        // 1 TEST: CARGA DE CONFIGURACIÓN GLOBAL
        // ============================================================
        std::cout << "\n=== TEST 1: CARGA DE CONFIGURACIÓN ===\n";
        YamlGameConfig cfg("../assets/config.yaml");

         // ============================================================
         // 2 TEST: CARGA DE MAPA DESDE YAML
         // ============================================================
         std::cout << "\n=== TEST 2: CARGA DE MAPA ===\n";
         Box2DPhysicsWorld world;
         RaceDefinition testRace{"LibertyCity", "../assets/liberty_city_circuito1.yaml"};
         std::vector<RaceDefinition> races{testRace};

         //  solo el mapa sin sesión:
        std::vector<std::unique_ptr<Wall>> walls;
        std::vector<std::unique_ptr<Bridge>> bridges;
        std::vector<Checkpoint> checkpoints;
        std::vector<Hint> hints;
        std::vector<SpawnPoint> spawnPoints;
         MapLoader::loadFromYAML(testRace.mapFile, world, walls, bridges, checkpoints, hints, spawnPoints);

         // ============================================================
         // 3 TEST: CREACIÓN DE AUTO Y PARTIDA SIMPLE
         // ============================================================

         std::cout << "\n=== TEST 3: CREACIÓN DE AUTO ===\n";

         PlayerConfig player1{1, "TestPlayer", "Speedster"};
         std::vector<PlayerConfig> players{player1};

         GameSessionFacade game(cfg);
         game.start(races, players);
         //game.stop();


        // ============================================================
        // 4 TEST: SIMULACIÓN DE EVENTOS DISCRETOS (como por red UDP)
        // ============================================================
        std::cout << "\n=== TEST 4: EVENTOS DISCRETOS ===\n";

        float dt = 1.0f / 60.0f;
        float totalTime = 0.0f;
        float maxTime = 20.0f;

        while (game.isRunning() && totalTime < maxTime) {
            // cada 2 segundos, alternar dirección
            if (static_cast<int>(totalTime) % 4 < 2)
                game.onPlayerEvent(1, "turn_right");
            else
                game.onPlayerEvent(1, "turn_left");

            // acelera los primeros 5 segundos
            if (totalTime < 5.0f)
                game.onPlayerEvent(1, "accelerate_down");
            else
                game.onPlayerEvent(1, "accelerate_up");

            game.update(dt);

            auto snap = game.getSnapshot();
            if (!snap.cars.empty()) {
                const auto& c = snap.cars[0];
                std::cout << "[t=" << totalTime << "s] "
                          << "pos=(" << c.x << "," << c.y
                          << ") vel=" << c.speed
                          << " angle=" << c.angle << "\n";
            }

            totalTime += dt;
        }
        return 0;
//         // ============================================================
//         // 5 TEST: SNAPSHOT ESTÁTICO (MAPA + OBJETOS)
//         // ============================================================
//         std::cout << "\n=== TEST 5: SNAPSHOT ESTÁTICO ===\n";
//         StaticSnapshot staticSnap = game.getStaticSnapshot();
//         std::cout << "Mapa: " << staticSnap.mapName << "\n";
//         std::cout << "Walls: " << staticSnap.walls.size() << "\n";
//         std::cout << "Checkpoints: " << staticSnap.checkpoints.size() << "\n";
//         std::cout << "Hints: " << staticSnap.hints.size() << "\n";
//         std::cout << "Spawns: " << staticSnap.spawnPoints.size() << "\n";
//
//         // ============================================================
//         // TEST: MÚLTIPLES JUGADORES (simulación mínima)
//         // ============================================================
//         // Descomentar para probar más de un auto.
//         /*
//         std::cout << "\n=== TEST 6: MULTIJUGADOR ===\n";
//         PlayerConfig p1{1, "Alice", "Tank", "Blue"};
//         PlayerConfig p2{2, "Bob", "Ghost", "Green"};
//         std::vector<PlayerConfig> multiPlayers{p1, p2};
//
//         GameSessionFacade multiGame(cfg);
//         multiGame.start(races, multiPlayers);
//
//         for (int frame = 0; frame < 300; ++frame) {
//             PlayerInput i1; i1.accelerate = true; i1.turn = "right";
//             PlayerInput i2; i2.accelerate = true; i2.turn = "left";
//             multiGame.applyInput(1, i1, dt);
//             multiGame.applyInput(2, i2, dt);
//             multiGame.update(dt);
//             if (frame % 30 == 0) {
//                 auto s = multiGame.getSnapshot();
//                 std::cout << "[frame " << frame << "] cars=" << s.cars.size() << "\n";
//             }
//         }
//         */
//
//         // ============================================================
//         // FIN
//         // ============================================================
//         std::cout << "\nTodas las pruebas completadas correctamente.\n";
    }
    catch (const std::exception& e) {
        std::cerr << " Error durante las pruebas: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
