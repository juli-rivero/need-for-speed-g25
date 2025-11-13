#pragma once

#include <SDL2/SDL.h>

#include <chrono>
#include <cmath>
#include <iostream>
#include <memory>
#include <thread>
#include <vector>

#include "config/MapLoader.h"
#include "config/YamlGameConfig.h"
#include "session/logic/GameSessionFacade.h"
#include "session/logic/types.h"
#include "session/model/Bridge.h"
#include "session/model/Wall.h"
#include "session/physics/Box2DPhysicsWorld.h"
#include "session/physics/EntityFactory.h"

// MAIN DE PRUEBAS MANUALES (FISICAS,LOGICA,SERVIDOR) --NO ES EL CLIENTE
// ============================================================

// ============================================================
static const int WIN_W = 1000;
static const int WIN_H = 700;
static const float PPM = 20.0f;  // pixels per meter
static const float DT = 1.0f / 60.0f;

static void SDL_RenderFillRectExF(SDL_Renderer* r, SDL_FRect* rect, float angle,
                                  SDL_FPoint* center, SDL_RendererFlip flip) {
    // Obtiene el color actual del renderer
    Uint8 r0, g0, b0, a0;
    SDL_GetRenderDrawColor(r, &r0, &g0, &b0, &a0);

    SDL_Texture* tex = SDL_CreateTexture(r, SDL_PIXELFORMAT_RGBA8888,
                                         SDL_TEXTUREACCESS_TARGET,
                                         static_cast<int>(rect->w),
                                         static_cast<int>(rect->h));

    // Renderizamos en la textura con el color activo
    SDL_SetRenderTarget(r, tex);
    SDL_SetRenderDrawColor(r, r0, g0, b0, a0);
    SDL_RenderClear(r);
    SDL_SetRenderTarget(r, nullptr);

    // Copiamos la textura rotada al renderer principal
    SDL_RenderCopyExF(r, tex, nullptr, rect, angle, center, flip);
    SDL_DestroyTexture(tex);
}

// ============================================================
// 🔸 Render utilities para test()
// ============================================================
static void renderWalls(SDL_Renderer* r, const StaticSnapshot& stat, float camX, float camY) {
    SDL_SetRenderDrawColor(r, 200, 40, 40, 255);
    for (const auto& wall : stat.walls) {
        SDL_FRect rect{ wall.x * PPM - wall.w * PPM / 2.0f - camX,
                        wall.y * PPM - wall.h * PPM / 2.0f - camY,
                        wall.w * PPM, wall.h * PPM };
        SDL_RenderFillRectF(r, &rect);
    }
}

static void renderBridges(SDL_Renderer* r, const StaticSnapshot& stat, float camX, float camY) {
    SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);

    for (const auto& br : stat.bridges) {
        SDL_FRect rectLower{ br.lowerX * PPM - br.w * PPM / 2.0f - camX,
                             br.lowerY * PPM - br.h * PPM / 2.0f - camY,
                             br.w * PPM, br.h * PPM };
        SDL_FRect rectUpper{ br.upperX * PPM - br.w * PPM / 2.0f - camX,
                             br.upperY * PPM - br.h * PPM / 2.0f - camY,
                             br.w * PPM, br.h * PPM };

        if (br.driveable) {
            SDL_SetRenderDrawColor(r, 100, 180, 255, 180);
            SDL_RenderFillRectF(r, &rectUpper);
            SDL_SetRenderDrawColor(r, 80, 140, 230, 255);
            SDL_RenderDrawRectF(r, &rectUpper);
        } else {
            SDL_SetRenderDrawColor(r, 60, 60, 180, 160);
            SDL_RenderFillRectF(r, &rectLower);
            SDL_SetRenderDrawColor(r, 40, 40, 150, 255);
            SDL_RenderDrawRectF(r, &rectLower);
        }
    }

    SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_NONE);
}

static void renderCheckpoints(SDL_Renderer* r, const StaticSnapshot& stat,
                              const std::vector<PlayerSnapshot>& players,
                              float camX, float camY)
{
    if (players.empty()) return;

    // Suponemos jugador local (id=1)
    int localId = 1;
    auto it = std::find_if(players.begin(), players.end(),
                           [&](const PlayerSnapshot& p){ return p.id == localId; });
    if (it == players.end()) return;

    int nextCP = it->raceProgress.nextCheckpoint;

    SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);

    for (const auto& cp : stat.checkpoints) {
        if (cp.order < nextCP) continue; // ya pasado → no renderizar

        SDL_FRect rect{ cp.x * PPM - cp.w * PPM / 2.0f - camX,
                        cp.y * PPM - cp.h * PPM / 2.0f - camY,
                        cp.w * PPM, cp.h * PPM };

        int alpha = (cp.order == nextCP) ? 200 : 80;
        SDL_SetRenderDrawColor(r, 0, 255, 120, alpha);
        SDL_RenderFillRectF(r, &rect);
        SDL_SetRenderDrawColor(r, 0, 200, 100, 255);
        SDL_RenderDrawRectF(r, &rect);
    }

    SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_NONE);
}


// static void renderCar(SDL_Renderer* r, const CarSnapshot& carDyn, const CarStaticInfo& carStat,
//                       float camX, float camY) {
//     float px = carDyn.x * PPM - camX;
//     float py = carDyn.y * PPM - camY;
//
//     SDL_FRect carRect{ px - carStat.width * PPM / 2.0f,
//                        py - carStat.height * PPM / 2.0f,
//                        carStat.width * PPM, carStat.height * PPM };
//     SDL_FPoint center = { carRect.w / 2, carRect.h / 2 };
//
//     SDL_SetRenderDrawColor(r, 40, 180, 240, 255);
//     SDL_RenderFillRectExF(r, &carRect, carDyn.angle * 180.0f / M_PI, &center, SDL_FLIP_NONE);
//
//     SDL_SetRenderDrawColor(r, 255, 255, 255, 255);
//     float noseLen = carRect.h * 0.5f;
//     float nx = px + std::cos(carDyn.angle) * noseLen;
//     float ny = py + std::sin(carDyn.angle) * noseLen;
//     SDL_RenderDrawLineF(r, px, py, nx, ny);
// }

static void renderUI(SDL_Renderer* r, const CarSnapshot& carDyn, const CarStaticInfo& carStat) {
    float uiX = 20.0f;
    float uiY = 20.0f;

    // vida
    float healthRatio = std::clamp(carDyn.health / 100.0f, 0.0f, 1.0f);
    SDL_FRect bgH = {uiX, uiY, 200.0f, 20.0f};
    SDL_FRect fgH = {uiX, uiY, 200.0f * healthRatio, 20.0f};
    SDL_SetRenderDrawColor(r, 60, 60, 60, 255);
    SDL_RenderFillRectF(r, &bgH);
    SDL_SetRenderDrawColor(r, 220, 50, 50, 255);
    SDL_RenderFillRectF(r, &fgH);
    SDL_SetRenderDrawColor(r, 255, 255, 255, 255);
    SDL_RenderDrawRectF(r, &bgH);

    // velocidad
    float speedRatio = std::clamp(carDyn.speed / carStat.maxSpeed, 0.0f, 1.0f);
    SDL_FRect bgS = {uiX, uiY + 30.0f, 200.0f, 20.0f};
    SDL_FRect fgS = {uiX, uiY + 30.0f, 200.0f * speedRatio, 20.0f};
    SDL_SetRenderDrawColor(r, 60, 60, 60, 255);
    SDL_RenderFillRectF(r, &bgS);
    SDL_SetRenderDrawColor(r, 40, 180, 255, 255);
    SDL_RenderFillRectF(r, &fgS);
    SDL_SetRenderDrawColor(r, 255, 255, 255, 255);
    SDL_RenderDrawRectF(r, &bgS);
}


int test() {
    try {
        if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
            std::cerr << "SDL_Init error: " << SDL_GetError() << "\n";
            return 1;
        }

        SDL_Window* window = SDL_CreateWindow(
            "Box2D Car Test", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
            WIN_W, WIN_H, SDL_WINDOW_SHOWN);
        SDL_Renderer* renderer =
            SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

        // --- configuración INICIAL y crear partida simple ---
        YamlGameConfig cfg("assets/config.yaml");

        //DURANTE EL LOBBY DEBERIA GENERAR ESTO
        RaceDefinition race{"LibertyCity",
                            "assets/liberty_city_circuito2.yaml"};
        PlayerConfig player{1, "Tester", "Speedster"};
        PlayerConfig player2{2, "Ghost",  "Ghost"};
        std::vector<RaceDefinition> races{race};
        std::vector<PlayerConfig> players{player,player2};


        GameSessionFacade game(cfg);
        game.start(races, players); //EMPEZAR PARTIDA EN EL LOBBY -> CAMBIAR A SDL

        auto stat = game.getStaticSnapshot();
        bool running = true;
        float nitroTime = 0.0f;
        bool nitroActive = false;


        SDL_Event e;
        while (running && game.isRunning()) {
            while (SDL_PollEvent(&e)) {
                if (e.type == SDL_QUIT) running = false;

                if (e.type == SDL_KEYDOWN && !e.key.repeat) {
                    switch (e.key.keysym.sym) {
                        case SDLK_ESCAPE:
                            running = false;
                            break;
                        case SDLK_w:
                            game.onPlayerEvent(1, "accelerate_down");
                            break;
                        case SDLK_s:
                            game.onPlayerEvent(1, "brake_down");
                            break;
                        case SDLK_a:
                            game.onPlayerEvent(1, "turn_left_down");
                            break;
                        case SDLK_d:
                            game.onPlayerEvent(1, "turn_right_down");
                            break;
                        case SDLK_LSHIFT:
                        case SDLK_RSHIFT:
                            game.onPlayerEvent(1, "nitro_toggle");
                            nitroActive = true;
                            nitroTime = 3.0f;  // 3 segundos de nitro
                            break;
                        case SDLK_SPACE:
                            game.onPlayerEvent(2, "accelerate_down");
                            break;
                    }
                }
                if (e.type == SDL_KEYUP && !e.key.repeat) {
                    switch (e.key.keysym.sym) {
                        case SDLK_w:
                            game.onPlayerEvent(1, "accelerate_up");
                            break;
                        case SDLK_s:
                            game.onPlayerEvent(1, "brake_up");
                            break;
                        case SDLK_a:
                            game.onPlayerEvent(1, "turn_left_up");
                            break;
                        case SDLK_d:
                            game.onPlayerEvent(1, "turn_right_up");
                            break;
                        case SDLK_SPACE:
                            game.onPlayerEvent(2, "accelerate_up");
                            break;
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


            // --- Actualiza lógica ---
            game.update(DT);
            auto dyn = game.getSnapshot();
            if (dyn.players.empty()) continue;

            // jugador local
            int localPlayerId = 1;
            auto itLocal = std::find_if(dyn.players.begin(), dyn.players.end(),
                [&](const PlayerSnapshot& p){ return p.id == localPlayerId; });
            if (itLocal == dyn.players.end()) continue;
            const auto& local = *itLocal;

            // Cámara centrada en jugador local
            float camX = local.car.x * PPM - WIN_W / 2.0f;
            float camY = local.car.y * PPM - WIN_H / 2.0f;

            // --- Render ---
            SDL_SetRenderDrawColor(renderer, 25, 25, 25, 255);
            SDL_RenderClear(renderer);

            renderWalls(renderer, stat, camX, camY);
            renderBridges(renderer, stat, camX, camY);
            renderCheckpoints(renderer, stat, dyn.players,camX, camY);
            for (const auto& ps : dyn.players) {
                std::cout << "Car " << ps.id << " pos=(" << ps.car.x << "," << ps.car.y << ")\n";
            }
            for (const auto& cp : stat.checkpoints) {
                std::cout << "CP order=" << cp.order
                          << " x=" << cp.x << " y=" << cp.y
                          << " w=" << cp.w << " h=" << cp.h << "\n";
            }

            //  todos los autos
            for (const auto& ps : dyn.players) {
                const auto& carDyn = ps.car;

                // Buscar info estática del auto correspondiente
                auto itStat = std::find_if(stat.cars.begin(), stat.cars.end(),
                    [&](const CarStaticInfo& c){ return c.id == ps.id; });
                if (itStat == stat.cars.end()) continue;
                const auto& carStat = *itStat;

                float px = carDyn.x * PPM - camX;
                float py = carDyn.y * PPM - camY;

                SDL_FRect carRect{
                    px - carStat.width * PPM / 2.0f,
                    py - carStat.height * PPM / 2.0f,
                    carStat.width * PPM,
                    carStat.height * PPM
                };
                SDL_FPoint center = {carRect.w / 2, carRect.h / 2};

                // Colores por jugador
                if (ps.id == localPlayerId) {
                    SDL_SetRenderDrawColor(renderer, 40,180,240,255); // celeste
                } else {
                    SDL_SetRenderDrawColor(renderer, 255,120,50,255); // naranja
                }

                SDL_RenderFillRectExF(renderer, &carRect,
                                      carDyn.angle * 180.0f / M_PI, &center, SDL_FLIP_NONE);

                // Línea de dirección
                SDL_SetRenderDrawColor(renderer, 255,255,255,255);
                float noseLen = carRect.h * 0.5f;
                float nx = px + std::cos(carDyn.angle) * noseLen;
                float ny = py + std::sin(carDyn.angle) * noseLen;
                SDL_RenderDrawLineF(renderer, px, py, nx, ny);

                // // ============================================================
                // //  Barra de vida sobre el auto
                // // ===========================================================
                // float barWidth = 40.0f;
                // float barHeight = 5.0f;
                // float offsetY = -carStat.height * PPM / 2.0f - 12.0f; // encima del auto
                //
                // float healthRatio = std::clamp(carDyn.health / 100.0f, 0.0f, 1.0f);
                //
                // // Color interpolado entre rojo (baja vida) y verde (salud)
                // Uint8 rColor = static_cast<Uint8>((1.0f - healthRatio) * 255);
                // Uint8 gColor = static_cast<Uint8>(healthRatio * 255);
                //
                // SDL_FRect bgBar = {px - barWidth / 2.0f, py + offsetY, barWidth, barHeight};
                // SDL_SetRenderDrawColor(renderer, 60, 60, 60, 180);
                // SDL_RenderFillRectF(renderer, &bgBar);
                //
                // SDL_FRect fgBar = {px - barWidth / 2.0f, py + offsetY,
                //                    barWidth * healthRatio, barHeight};
                // SDL_SetRenderDrawColor(renderer, rColor, gColor, 40, 255);
                // SDL_RenderFillRectF(renderer, &fgBar);
                //
                // SDL_SetRenderDrawColor(renderer, 255, 255, 255, 200);
                // SDL_RenderDrawRectF(renderer, &bgBar);



            }



            // ============================================================
            // 🔹 Dibujar Hint direccional (flecha al siguiente checkpoint)
            // ============================================================
            if (!dyn.players.empty()) {
                const auto& locall = *itLocal;
                int nextCP = locall.raceProgress.nextCheckpoint;

                if (nextCP < (int)stat.checkpoints.size()) {
                    const auto& cp = stat.checkpoints[nextCP];

                    // centro del auto
                    float px = locall.car.x * PPM - camX;
                    float py = locall.car.y * PPM - camY;

                    // vector hacia el checkpoint
                    float dx = (cp.x - locall.car.x);
                    float dy = (cp.y - locall.car.y);
                    float len = std::sqrt(dx * dx + dy * dy);
                    if (len > 0.001f) { dx /= len; dy /= len; }

                    // frente del auto
                    float angle = locall.car.angle;
                    float fx = px + std::cos(angle) * 25;
                    float fy = py + std::sin(angle) * 25;

                    // línea principal (flecha)
                    SDL_SetRenderDrawColor(renderer, 255, 220, 0, 255);
                    SDL_RenderDrawLineF(renderer, fx, fy, fx + dx * 30, fy + dy * 30);

                    // cabeza triangular
                    SDL_FPoint tip = {fx + dx * 30, fy + dy * 30};
                    SDL_RenderDrawLineF(renderer, tip.x, tip.y,
                                        tip.x - dy * 5, tip.y + dx * 5);
                    SDL_RenderDrawLineF(renderer, tip.x, tip.y,
                                        tip.x + dy * 5, tip.y - dx * 5);
                }
            }


            // Dibujar UI (vida y velocidad) del jugador local
            auto itStatLocal = std::find_if(stat.cars.begin(), stat.cars.end(),
                [&](const CarStaticInfo& c){ return c.id == local.id; });
            if (itStatLocal != stat.cars.end()) {
                renderUI(renderer, local.car, *itStatLocal);
            }

            SDL_RenderPresent(renderer);
            SDL_Delay((int)(DT * 1000));
        }

        game.stop();
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 0;
    } catch (const std::exception& e) {
        std::cerr << " Error durante las pruebas: " << e.what() << std::endl;
        return 1;
    }
}
