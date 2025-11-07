#include <iostream>
#include <memory>
#include <vector>
#include <thread>
#include <chrono>
#include <cmath>
#include <SDL2/SDL.h>
#include "session/model/Bridge.h"
#include "session/model/Wall.h"
#include "session/logic/types.h"
#include "config/MapLoader.h"
#include "config/YamlGameConfig.h"
#include "session/logic/GameSessionFacade.h"
#include "session/physics/Box2DPhysicsWorld.h"
#include "session/physics/EntityFactory.h"


// ============================================================
// MAIN DE PRUEBAS MANUALES (FISICAS,LOGICA,SERVIDOR) --NO ES EL CLIENTE
// ============================================================

// ============================================================
static const int   WIN_W = 1000;
static const int   WIN_H = 700;
static const float PPM   = 20.0f; // pixels per meter
static const float DT    = 1.0f / 60.0f;

static void SDL_RenderFillRectExF(SDL_Renderer* r, SDL_FRect* rect, float angle, SDL_FPoint* center, SDL_RendererFlip flip) {
    SDL_Texture* tex = SDL_CreateTexture(r, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, (int)rect->w, (int)rect->h);
    SDL_SetRenderTarget(r, tex);
    SDL_SetRenderDrawColor(r, 40, 180, 240, 255);
    SDL_RenderClear(r);
    SDL_SetRenderTarget(r, nullptr);
    SDL_RenderCopyExF(r, tex, nullptr, rect, angle, center, flip);
    SDL_DestroyTexture(tex);
}
int main() {
    try {
//         // ============================================================
//         // 1 TEST: CARGA DE CONFIGURACIÓN GLOBAL
//         // ============================================================
//         std::cout << "\n=== TEST 1: CARGA DE CONFIGURACIÓN ===\n";
//         YamlGameConfig cfg("assets/config.yaml");
//
//          // ============================================================
//          // 2 TEST: CARGA DE MAPA DESDE YAML
//          // ============================================================
//          std::cout << "\n=== TEST 2: CARGA DE MAPA ===\n";
//          Box2DPhysicsWorld world;
//          RaceDefinition testRace{"LibertyCity", "assets/liberty_city_circuito1.yaml"};
//          std::vector<RaceDefinition> races{testRace};
//
//          //  solo el mapa sin sesión:
//         std::vector<std::unique_ptr<Wall>> walls;
//         std::vector<std::unique_ptr<Bridge>> bridges;
//         std::vector<Checkpoint> checkpoints;
//         std::vector<Hint> hints;
//         std::vector<SpawnPoint> spawnPoints;
//          MapLoader::loadFromYAML(testRace.mapFile, world, walls, bridges, checkpoints, hints, spawnPoints);
//
//          // ============================================================
//          // 3 TEST: CREACIÓN DE AUTO Y PARTIDA SIMPLE
//          // ============================================================
//
//          std::cout << "\n=== TEST 3: CREACIÓN DE AUTO ===\n";
//
//          PlayerConfig player1{1, "TestPlayer", "Speedster"};
//          std::vector<PlayerConfig> players{player1};
//
//          GameSessionFacade game(cfg);
//          game.start(races, players);
//          //game.stop();
//
//
//         // ============================================================
//         // 4 TEST: SIMULACIÓN DE EVENTOS DISCRETOS (como por red UDP)
//         // ============================================================
//         std::cout << "\n=== TEST 4: EVENTOS DISCRETOS ===\n";
//
//         float dt = 1.0f / 60.0f;
//         float totalTime = 0.0f;
//         float maxTime = 10.0f;
//
//         while (game.isRunning() && totalTime < maxTime) {
//             // Cada 2 segundos alterna entre girar a derecha e izquierda
//             bool right = (static_cast<int>(totalTime) % 4 < 2);
//             if (right) {
//                 game.onPlayerEvent(1, "turn_right_down");
//                 game.onPlayerEvent(1, "turn_left_up");
//             } else {
//                 game.onPlayerEvent(1, "turn_left_down");
//                 game.onPlayerEvent(1, "turn_right_up");
//             }
//
//             // acelera los primeros 5 segundos
//             if (totalTime < 5.0f) {
//                 game.onPlayerEvent(1, "accelerate_down");
//             } else {
//                 game.onPlayerEvent(1, "accelerate_up");
// }
//
//             game.update(dt);
//
//             auto snap = game.getSnapshot();
//             if (!snap.cars.empty()) {
//                 const auto& c = snap.cars[0];
//                 std::cout << "[t=" << totalTime << "s] "
//                           << "pos=(" << c.x << "," << c.y
//                           << ") vel=" << c.speed
//                           << " angle=" << c.angle << "\n";
//             }
//
//             totalTime += dt;
//         }
//         return 0;

if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
        std::cerr << "SDL_Init error: " << SDL_GetError() << "\n";
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow(
        "Box2D Car Test",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WIN_W, WIN_H, SDL_WINDOW_SHOWN
    );
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    // --- Cargar configuración y crear partida simple ---
    YamlGameConfig cfg("assets/config.yaml");
    RaceDefinition race{"LibertyCity", "assets/liberty_city_circuito1.yaml"};
    PlayerConfig player{1, "Tester", "Speedster"};
    std::vector<RaceDefinition> races{race};
    std::vector<PlayerConfig> players{player};

    GameSessionFacade game(cfg);
    game.start(races, players);

    bool running = true;
    float totalTime = 0.0f;
    float nitroTime = 0.0f;
    bool nitroActive = false;
    auto stat = game.getStaticSnapshot();
    const auto& carStat = stat.cars[0]; // mismo orden o ID
    SDL_Event e;
    while (running && game.isRunning()) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) running = false;

            if (e.type == SDL_KEYDOWN && !e.key.repeat) {
                switch (e.key.keysym.sym) {
                    case SDLK_ESCAPE: running = false; break;
                    case SDLK_w: game.onPlayerEvent(1, "accelerate_down"); break;
                    case SDLK_s: game.onPlayerEvent(1, "brake_down"); break;
                    case SDLK_a: game.onPlayerEvent(1, "turn_left_down"); break;
                    case SDLK_d: game.onPlayerEvent(1, "turn_right_down"); break;
                    case SDLK_LSHIFT:
                    case SDLK_RSHIFT:
                        game.onPlayerEvent(1, "nitro_toggle");
                        nitroActive = true;
                        nitroTime = 3.0f; // 3 segundos de nitro
                        break;
                }
            }
            if (e.type == SDL_KEYUP && !e.key.repeat) {
                switch (e.key.keysym.sym) {
                    case SDLK_w: game.onPlayerEvent(1, "accelerate_up"); break;
                    case SDLK_s: game.onPlayerEvent(1, "brake_up"); break;
                    case SDLK_a: game.onPlayerEvent(1, "turn_left_up"); break;
                    case SDLK_d: game.onPlayerEvent(1, "turn_right_up"); break;
                }
            }
        }

        // Control del nitro (se apaga automáticamente tras X segundos)
        if (nitroActive) {
            nitroTime -= DT;
            if (nitroTime <= 0) {
                nitroActive = false;
                game.onPlayerEvent(1, "nitro_toggle");
            }
        }

        // Actualizar simulación
        game.update(DT);
        totalTime += DT;

        // Obtener snapshot para render
        auto dyn = game.getSnapshot();
        if (dyn.cars.empty()) continue;


        // --- Encontrar el auto del jugador ---
        const auto& carDyn = dyn.cars[0];
        // --- Calcular cámara centrada en el auto ---
        float camX = carDyn.x * PPM - WIN_W / 2.0f;
        float camY = carDyn.y * PPM - WIN_H / 2.0f;

        // --- Render ---
        SDL_SetRenderDrawColor(renderer, 25, 25, 25, 255);
        SDL_RenderClear(renderer);

        // --- Dibujar muros ---
        // --- Dibujar muros ---
        SDL_SetRenderDrawColor(renderer, 200, 40, 40, 255);
        for (const auto& wall : stat.walls) {
            SDL_FRect rect;
            rect.w = wall.w * PPM;
            rect.h = wall.h * PPM;

            // Los muros están definidos por su centro → convertir a esquina superior izquierda
            rect.x = wall.x * PPM - rect.w / 2.0f - camX;
            rect.y = wall.y * PPM - rect.h / 2.0f - camY;

            SDL_RenderFillRectF(renderer, &rect);
        }
        // Convertir coordenadas físicas → píxeles
        float px = carDyn.x * PPM - camX;
        float py = carDyn.y * PPM - camY;

        SDL_FRect carRect;
        carRect.w = carStat.width * PPM; // igual a tu shape físico
        carRect.h = carStat.height * PPM;
        carRect.x = px - carRect.w / 2;
        carRect.y = py - carRect.h / 2;

        SDL_FPoint center = { carRect.w / 2, carRect.h / 2 };

        float angleDeg = carDyn.angle * 180.0f / M_PI;

        SDL_SetRenderDrawColor(renderer, 40, 180, 240, 255);
        SDL_RenderFillRectExF(renderer, &carRect, angleDeg, &center, SDL_FLIP_NONE);

        // Dibujar “frente” del auto (una línea para ver orientación)
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        float noseLen = carRect.h * 0.5f;
        float nx = px + std::cos(carDyn.angle) * noseLen;
        float ny = py + std::sin(carDyn.angle) * noseLen;
        SDL_RenderDrawLineF(renderer, px, py, nx, ny);

        // ============================================================
        //  UI SIMPLE: VIDA Y VELOCIDAD
        // ============================================================
        float uiX = 20.0f;
        float uiY = 20.0f;

        // ---- VIDA ----
        float healthRatio = carDyn.health / 100.0f; // asumimos 100 vida
        healthRatio = std::clamp(healthRatio, 0.0f, 1.0f);
        float healthWidth = 200.0f * healthRatio;

        // fondo gris
        SDL_FRect bgHealth = { uiX, uiY, 200.0f, 20.0f };
        SDL_SetRenderDrawColor(renderer, 60, 60, 60, 255);
        SDL_RenderFillRectF(renderer, &bgHealth);

        // barra roja
        SDL_FRect fgHealth = { uiX, uiY, healthWidth, 20.0f };
        SDL_SetRenderDrawColor(renderer, 220, 50, 50, 255);
        SDL_RenderFillRectF(renderer, &fgHealth);

        // borde
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderDrawRectF(renderer, &bgHealth);

        // ---- VELOCIDAD ----
        float speedRatio = carDyn.speed / carStat.maxSpeed;
        speedRatio = std::clamp(speedRatio, 0.0f, 1.0f);
        float speedWidth = 200.0f * speedRatio;

        SDL_FRect bgSpeed = { uiX, uiY + 30.0f, 200.0f, 20.0f };
        SDL_FRect fgSpeed = { uiX, uiY + 30.0f, speedWidth, 20.0f };

        // fondo gris
        SDL_SetRenderDrawColor(renderer, 60, 60, 60, 255);
        SDL_RenderFillRectF(renderer, &bgSpeed);

        // barra azul
        SDL_SetRenderDrawColor(renderer, 40, 180, 255, 255);
        SDL_RenderFillRectF(renderer, &fgSpeed);

        // borde
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderDrawRectF(renderer, &bgSpeed);

        // imprimir en consola (opcional)
        std::cout << "Vida: " << carDyn.health << " | Velocidad: "
                  << carDyn.speed *3.6f<< "/" << carStat.maxSpeed*3.6f << std::endl;




        SDL_RenderPresent(renderer);
        SDL_Delay((int)(DT * 1000));
    }

    game.stop();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
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
