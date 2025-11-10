#pragma once

#include <SDL2/SDL.h>

#include <chrono>
#include <cmath>
#include <iostream>
#include <memory>
#include <thread>
#include <vector>

#include "common/structs.h"
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
    SDL_Texture* tex =
        SDL_CreateTexture(r, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET,
                          static_cast<int>(rect->w), static_cast<int>(rect->h));
    SDL_SetRenderTarget(r, tex);
    SDL_SetRenderDrawColor(r, 40, 180, 240, 255);
    SDL_RenderClear(r);
    SDL_SetRenderTarget(r, nullptr);
    SDL_RenderCopyExF(r, tex, nullptr, rect, angle, center, flip);
    SDL_DestroyTexture(tex);
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

        // --- Cargar configuración y crear partida simple ---
        YamlGameConfig cfg("assets/config.yaml");
        RaceDefinition race{"LibertyCity",
                            "assets/liberty_city_circuito1.yaml"};
        PlayerConfig player{1, "Tester", "Speedster"};
        std::vector<RaceDefinition> races{race};
        std::vector<PlayerConfig> players{player};

        GameSessionFacade game(cfg);
        game.start(races, players);

        bool running = true;

        float nitroTime = 0.0f;
        bool nitroActive = false;
        auto stat = game.getStaticSnapshot();
        const auto& carStat = stat.cars[0];  // mismo orden o ID
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

            try {
                game.update(DT);
            } catch (const std::exception& ex) {
                std::cerr << "[EXCEPCIÓN] " << ex.what() << std::endl;
                break;
            }

            auto dyn = game.getSnapshot();
            if (dyn.players.empty()) continue;

            // Buscar el jugador local (por id)
            int localPlayerId = 1;  // o el ID real de tu jugador "Tester"
            auto it = std::find_if(
                dyn.players.begin(), dyn.players.end(),
                [&](const PlayerSnapshot& p) { return p.id == localPlayerId; });

            if (it == dyn.players.end()) continue;  // por seguridad

            const auto& playerr = *it;
            const auto& carDyn = playerr.car;  // snapshot del auto ---
            float camX = carDyn.x * PPM - WIN_W / 2.0f;
            float camY = carDyn.y * PPM - WIN_H / 2.0f;

            // --- Render ---
            SDL_SetRenderDrawColor(renderer, 25, 25, 25, 255);
            SDL_RenderClear(renderer);

            SDL_SetRenderDrawColor(renderer, 200, 40, 40, 255);
            for (const auto& wall : stat.walls) {
                SDL_FRect rect;
                rect.w = wall.w * PPM;
                rect.h = wall.h * PPM;

                // Los muros están definidos por su centro → convertir a esquina
                // superior izquierda
                rect.x = wall.x * PPM - rect.w / 2.0f - camX;
                rect.y = wall.y * PPM - rect.h / 2.0f - camY;

                SDL_RenderFillRectF(renderer, &rect);
            }  // ============================================================
            // 🔹 Dibujar BRIDGES (puentes)
            // ============================================================
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

            SDL_SetRenderDrawColor(renderer, 100, 100, 255,
                                   100);  // azul translúcido
            for (const auto& br : stat.bridges) {
                SDL_FRect rectLower, rectUpper;

                rectLower.w = rectUpper.w = br.w * PPM;
                rectLower.h = rectUpper.h = br.h * PPM;

                rectLower.x = br.lowerX * PPM - rectLower.w / 2.0f - camX;
                rectLower.y = br.lowerY * PPM - rectLower.h / 2.0f - camY;
                rectUpper.x = br.upperX * PPM - rectUpper.w / 2.0f - camX;
                rectUpper.y = br.upperY * PPM - rectUpper.h / 2.0f - camY;

                if (br.driveable) {
                    // Si es transitable → mostrar el superior más brillante
                    SDL_SetRenderDrawColor(renderer, 100, 180, 255, 180);
                    SDL_RenderFillRectF(renderer, &rectUpper);
                    SDL_SetRenderDrawColor(renderer, 80, 140, 230, 255);
                    SDL_RenderDrawRectF(renderer, &rectUpper);
                } else {
                    // No transitable → mostrar el inferior más visible
                    SDL_SetRenderDrawColor(renderer, 60, 60, 180, 160);
                    SDL_RenderFillRectF(renderer, &rectLower);
                    SDL_SetRenderDrawColor(renderer, 40, 40, 150, 255);
                    SDL_RenderDrawRectF(renderer, &rectLower);
                }
            }

            // ============================================================
            // 🔹 Dibujar CHECKPOINTS
            // ============================================================
            for (const auto& cp : stat.checkpoints) {
                SDL_FRect rect;
                rect.w = cp.w * PPM;
                rect.h = cp.h * PPM;
                rect.x = cp.x * PPM - rect.w / 2.0f - camX;
                rect.y = cp.y * PPM - rect.h / 2.0f - camY;

                int alpha = 70 + (cp.order * 15);
                if (alpha > 180) alpha = 180;

                SDL_SetRenderDrawColor(renderer, 0, 255, 100,
                                       alpha);  // verde suave
                SDL_RenderFillRectF(renderer, &rect);
                SDL_SetRenderDrawColor(renderer, 0, 180, 80, 200);
                SDL_RenderDrawRectF(renderer, &rect);
            }

            // ============================================================
            // 🔹 Dibujar HINTS
            // ============================================================
            for (const auto& hint : stat.hints) {
                float radius = 0.4f * PPM;  // tamaño visible
                float cx = hint.x * PPM - camX;
                float cy = hint.y * PPM - camY;

                // círculo simple (polígono aproximado)
                SDL_SetRenderDrawColor(renderer, 255, 200, 0, 180);
                const int segments = 12;
                for (int i = 0; i < segments; ++i) {
                    float a1 = (i / static_cast<float>(segments)) * 2 * M_PI;
                    float a2 =
                        ((i + 1) / static_cast<float>(segments)) * 2 * M_PI;
                    float x1 = cx + std::cos(a1) * radius;
                    float y1 = cy + std::sin(a1) * radius;
                    float x2 = cx + std::cos(a2) * radius;
                    float y2 = cy + std::sin(a2) * radius;
                    SDL_RenderDrawLineF(renderer, x1, y1, x2, y2);
                }
            }
            // --- Dibujar checkpoints ---
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

            for (const auto& cp : stat.checkpoints) {
                SDL_FRect rect;
                rect.w = cp.w * PPM;
                rect.h = cp.h * PPM;

                // coordenadas físicas → píxeles
                rect.x = cp.x * PPM - rect.w / 2.0f - camX;
                rect.y = cp.y * PPM - rect.h / 2.0f - camY;

                // color según el orden (más claros al inicio)
                int alpha = 60 + (cp.order * 20);
                if (alpha > 180) alpha = 180;

                SDL_SetRenderDrawColor(renderer, 0, 255, 120, alpha);
                SDL_RenderFillRectF(renderer, &rect);

                // borde más fuerte para distinguirlo
                SDL_SetRenderDrawColor(renderer, 0, 200, 100, 255);
                SDL_RenderDrawRectF(renderer, &rect);

                // línea de dirección del checkpoint (opcional)
                float cx = rect.x + rect.w / 2.0f;
                float cy = rect.y + rect.h / 2.0f;
                float len = rect.h * 0.8f;
                float nx =
                    cx + std::cos(cp.order * 0.2f) * len;  // solo estética
                float ny = cy + std::sin(cp.order * 0.2f) * len;
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 80);
                SDL_RenderDrawLineF(renderer, cx, cy, nx, ny);
            }
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
            float px = carDyn.x * PPM - camX;
            float py = carDyn.y * PPM - camY;

            SDL_FRect carRect;
            carRect.w = carStat.width * PPM;  // igual a tu shape físico
            carRect.h = carStat.height * PPM;
            carRect.x = px - carRect.w / 2;
            carRect.y = py - carRect.h / 2;

            SDL_FPoint center = {carRect.w / 2, carRect.h / 2};

            float angleDeg = carDyn.angle * 180.0f / M_PI;

            SDL_SetRenderDrawColor(renderer, 40, 180, 240, 255);
            SDL_RenderFillRectExF(renderer, &carRect, angleDeg, &center,
                                  SDL_FLIP_NONE);

            // Dibuja “frente” del auto (una línea para ver orientación)
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
            float healthRatio = carDyn.health / 100.0f;  // asumimos 100 vida
            healthRatio = std::clamp(healthRatio, 0.0f, 1.0f);
            float healthWidth = 200.0f * healthRatio;

            // fondo gris
            SDL_FRect bgHealth = {uiX, uiY, 200.0f, 20.0f};
            SDL_SetRenderDrawColor(renderer, 60, 60, 60, 255);
            SDL_RenderFillRectF(renderer, &bgHealth);

            // barra roja
            SDL_FRect fgHealth = {uiX, uiY, healthWidth, 20.0f};
            SDL_SetRenderDrawColor(renderer, 220, 50, 50, 255);
            SDL_RenderFillRectF(renderer, &fgHealth);

            // borde
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderDrawRectF(renderer, &bgHealth);

            // ---- VELOCIDAD ----
            float speedRatio = carDyn.speed / carStat.maxSpeed;
            speedRatio = std::clamp(speedRatio, 0.0f, 1.0f);
            float speedWidth = 200.0f * speedRatio;

            SDL_FRect bgSpeed = {uiX, uiY + 30.0f, 200.0f, 20.0f};
            SDL_FRect fgSpeed = {uiX, uiY + 30.0f, speedWidth, 20.0f};

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
            std::cout << "Vida: " << carDyn.health
                      << " | Velocidad: " << carDyn.speed * 3.6f << "/"
                      << carStat.maxSpeed * 3.6f << std::endl;

            SDL_RenderPresent(renderer);
            SDL_Delay(static_cast<int>(DT * 1000));
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
