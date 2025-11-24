#pragma once

#include <SDL2/SDL.h>
#include <SDL_ttf.h>

#include <cmath>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "common/structs.h"
#include "config/YamlGameConfig.h"
#include "session/logic/GameSessionFacade.h"
#include "session/logic/types.h"
#include "session/model/BridgeSensor.h"

#if OFFLINE

// MAIN DE PRUEBAS MANUALES (FISICAS,LOGICA,SERVIDOR) --NO ES EL CLIENTE
static const int WIN_W = 1000;
static const int WIN_H = 700;
static const float PPM = 10.0f;  // pixels per meter
static const float DT = 1.0f / 60.0f;

static const char* toString(MatchState s) {
    switch (s) {
        case MatchState::Starting:
            return "Starting";
        case MatchState::Racing:
            return "Racing";
        case MatchState::Intermission:
            return "Intermission";
        case MatchState::Finished:
            return "Finished";
    }
    return "Unknown";
}

static const char* toString(RaceState s) {
    switch (s) {
        case RaceState::Countdown:
            return "Countdown";
        case RaceState::Running:
            return "Running";
        case RaceState::Finished:
            return "Finished";
    }
    return "Unknown";
}

static void drawText(SDL_Renderer* renderer, TTF_Font* font, int x, int y,
                     const std::string& text,
                     SDL_Color color = {255, 255, 255, 255}) {
    SDL_Surface* surf = TTF_RenderText_Blended(font, text.c_str(), color);
    if (!surf) return;

    SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);
    SDL_FreeSurface(surf);
    if (!tex) return;

    SDL_Rect dst;
    dst.x = x;
    dst.y = y;
    SDL_QueryTexture(tex, nullptr, nullptr, &dst.w, &dst.h);
    SDL_RenderCopy(renderer, tex, nullptr, &dst);

    SDL_DestroyTexture(tex);
}

static void SDL_RenderFillRectExF(SDL_Renderer* r, SDL_FRect* rect, float angle,
                                  SDL_FPoint* center, SDL_RendererFlip flip) {
    // Obtiene el color actual del renderer
    Uint8 r0, g0, b0, a0;
    SDL_GetRenderDrawColor(r, &r0, &g0, &b0, &a0);

    SDL_Texture* tex =
        SDL_CreateTexture(r, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET,
                          static_cast<int>(rect->w), static_cast<int>(rect->h));

    // Renderizamos en la textura con el color activo
    SDL_SetRenderTarget(r, tex);
    SDL_SetRenderDrawColor(r, r0, g0, b0, a0);
    SDL_RenderClear(r);
    SDL_SetRenderTarget(r, nullptr);

    // Copiamos la textura rotada al renderer principal
    SDL_RenderCopyExF(r, tex, nullptr, rect, angle, center, flip);
    SDL_DestroyTexture(tex);
}
static void renderMiniHUD(SDL_Renderer* r, TTF_Font* font,
                          const WorldSnapshot& snap) {
    int x = 20;
    int y = WIN_H - 150;

    // Fondo oscuro
    SDL_SetRenderDrawColor(r, 20, 20, 20, 180);
    SDL_Rect bg = {x - 10, y - 10, 350, 140};
    SDL_RenderFillRect(r, &bg);

    SDL_Color white{255, 255, 255, 255};
    SDL_Color yellow{255, 220, 0, 255};

    drawText(r, font, x, y,
             std::string("Match State: ") + toString(snap.matchState), white);

    drawText(r, font, x, y + 20,
             std::string("Race State:  ") + toString(snap.raceState), white);

    drawText(r, font, x, y + 40,
             "Countdown:   " + std::to_string(snap.raceCountdown), yellow);

    drawText(r, font, x, y + 60,
             "Race Time:   " + std::to_string(snap.raceElapsed), yellow);

    drawText(r, font, x, y + 80,
             "Time Left:   " + std::to_string(snap.raceTimeLeft), yellow);

    drawText(r, font, x, y + 100,
             "Race Index:  " + std::to_string(snap.currentRaceIndex), white);

    drawText(r, font, x, y + 120,
             "Players:     " + std::to_string(snap.players.size()), white);
}

// ============================================================
//  Render utilities para test()
// ============================================================
static void renderBuildings(SDL_Renderer* r, const StaticSnapshot& stat,
                            float camX, float camY) {
    for (size_t i = 0; i < stat.walls.size(); ++i) {
        const auto& b = stat.walls[i];

        SDL_FRect rect{b.x * PPM - b.w * PPM * 0.5f - camX,
                       b.y * PPM - b.h * PPM * 0.5f - camY, b.w * PPM,
                       b.h * PPM};

        // Residential → rojo
        // Railing → amarillo
        if (i < stat.walls.size()) {
            SDL_SetRenderDrawColor(r, 200, 40, 40, 255);  // rojo residencial
        }
        if (/* es railing */ b.type == EntityType::Railing) {
            SDL_SetRenderDrawColor(r, 245, 220, 40, 255);  // amarillo railing
        }

        SDL_RenderFillRectF(r, &rect);
    }
}

// Puentes dibujados *debajo* de autos (para sombra / piso bajo)
static void renderBridgesBelowCars(SDL_Renderer* r, const StaticSnapshot& stat,
                                   float camX, float camY) {
    SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);

    for (const auto& br : stat.bridges) {
        SDL_FRect rect{br.x * PPM - br.w * PPM * 0.5f - camX,
                       br.y * PPM - br.h * PPM * 0.5f - camY, br.w * PPM,
                       br.h * PPM};

        SDL_SetRenderDrawColor(r, 40, 40, 40, 150);
        SDL_RenderFillRectF(r, &rect);
    }

    SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_NONE);
}

static void renderBridgeDeckAboveUnderCars(SDL_Renderer* r,
                                           const StaticSnapshot& stat,
                                           float camX, float camY) {
    SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);

    for (const auto& br : stat.bridges) {
        SDL_FRect rect{br.x * PPM - br.w * PPM * 0.5f - camX,
                       br.y * PPM - br.h * PPM * 0.5f - camY, br.w * PPM,
                       br.h * PPM};

        SDL_SetRenderDrawColor(r, 120, 190, 255, 220);  // celeste deck
        SDL_RenderFillRectF(r, &rect);
        SDL_SetRenderDrawColor(r, 80, 150, 220, 255);
        SDL_RenderDrawRectF(r, &rect);
    }

    SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_NONE);
}

static void renderOverpassesOnTop(SDL_Renderer* r, const StaticSnapshot& stat,
                                  float camX, float camY) {
    SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);

    for (const auto& op : stat.overpasses) {
        SDL_FRect rect{op.x * PPM - op.w * PPM * 0.5f - camX,
                       op.y * PPM - op.h * PPM * 0.5f - camY, op.w * PPM,
                       op.h * PPM};

        SDL_SetRenderDrawColor(r, 200, 80, 255, 200);  // violeta
        SDL_RenderFillRectF(r, &rect);
    }

    SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_NONE);
}
static void renderSensors(
    SDL_Renderer* r, const std::vector<std::unique_ptr<BridgeSensor>>& sensors,
    float camX, float camY) {
    SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);

    for (const auto& s : sensors) {
        const auto pos = s->getPosition();
        const auto w = s->getWidth();
        const auto h = s->getHeight();

        SDL_FRect rect{pos.x * PPM - (w * PPM) / 2 - camX,
                       pos.y * PPM - (h * PPM) / 2 - camY, w * PPM, h * PPM};

        if (s->getType() == BridgeSensorType::SetUpper) {
            SDL_SetRenderDrawColor(r, 0, 200, 255, 120);  // celeste claro
        } else {
            SDL_SetRenderDrawColor(r, 255, 140, 0, 120);  // naranja
        }

        SDL_RenderFillRectF(r, &rect);

        SDL_SetRenderDrawColor(r, 255, 255, 255, 200);
        SDL_RenderDrawRectF(r, &rect);
    }

    SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_NONE);
}
static void renderRailingBelowCars(SDL_Renderer* r, const StaticSnapshot& stat,
                                   float camX, float camY) {
    for (const auto& w : stat.walls) {
        if (w.type != EntityType::Railing) continue;

        SDL_FRect rect{w.x * PPM - w.w * PPM * 0.5f - camX,
                       w.y * PPM - w.h * PPM * 0.5f - camY, w.w * PPM,
                       w.h * PPM};

        // Amarillo fuerte (igual que siempre)
        SDL_SetRenderDrawColor(r, 245, 220, 40, 255);
        SDL_RenderFillRectF(r, &rect);
    }
}

static void renderRailingAboveCars(SDL_Renderer* r, const StaticSnapshot& stat,
                                   float camX, float camY) {
    SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);

    for (const auto& w : stat.walls) {
        if (w.type != EntityType::Railing) continue;

        SDL_FRect rect{w.x * PPM - w.w * PPM * 0.5f - camX,
                       w.y * PPM - w.h * PPM * 0.5f - camY, w.w * PPM,
                       w.h * PPM};

        // Amarillo con alpha (translúcido)
        SDL_SetRenderDrawColor(r, 245, 220, 40, 200);
        SDL_RenderFillRectF(r, &rect);
    }

    SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_NONE);
}
static void drawRotatedRect(SDL_Renderer* r, float cx, float cy, float w,
                            float h, float angleDeg, SDL_Color col) {
    float angleRad = angleDeg * M_PI / 180.0f;
    float hw = w * 0.5f;
    float hh = h * 0.5f;

    float cs = cosf(angleRad);
    float sn = sinf(angleRad);

    SDL_FPoint pts[4];

    pts[0] = {cx + (-hw * cs - -hh * sn), cy + (-hw * sn + -hh * cs)};

    pts[1] = {cx + (hw * cs - -hh * sn), cy + (hw * sn + -hh * cs)};

    pts[2] = {cx + (hw * cs - hh * sn), cy + (hw * sn + hh * cs)};

    pts[3] = {cx + (-hw * cs - hh * sn), cy + (-hw * sn + hh * cs)};

    SDL_SetRenderDrawColor(r, col.r, col.g, col.b, col.a);

    SDL_RenderDrawLineF(r, pts[0].x, pts[0].y, pts[1].x, pts[1].y);
    SDL_RenderDrawLineF(r, pts[1].x, pts[1].y, pts[2].x, pts[2].y);
    SDL_RenderDrawLineF(r, pts[2].x, pts[2].y, pts[3].x, pts[3].y);
    SDL_RenderDrawLineF(r, pts[3].x, pts[3].y, pts[0].x, pts[0].y);
}
static void renderCheckpoints(SDL_Renderer* r, const StaticSnapshot& stat,
                              const std::vector<PlayerSnapshot>& players,
                              float camX, float camY) {
    if (players.empty()) return;

    // Jugador local (id 1 para test)
    PlayerId localId = 1;
    auto it =
        std::find_if(players.begin(), players.end(),
                     [&](const PlayerSnapshot& p) { return p.id == localId; });
    if (it == players.end()) return;

    uint32_t nextCP = it->raceProgress.nextCheckpoint;

    SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);

    for (const auto& cp : stat.checkpoints) {
        if (cp.order < nextCP) continue;  // ya pasado → no dibujar

        // -----------------------------------------
        //  Rect base
        // -----------------------------------------
        float rw = cp.w * PPM;
        float rh = cp.h * PPM;

        // grosor mínimo para debug visual
        if (rh < 6) rh = 6;

        SDL_FRect rect{cp.x * PPM - rw * 0.5f - camX,
                       cp.y * PPM - rh * 0.5f - camY, rw, rh};

        // -----------------------------------------
        //   COLOR según tipo
        // -----------------------------------------
        SDL_Color col;

        switch (cp.type) {
            case CheckpointType::Start:
                col = {40, 220, 40, 255};  // verde
                break;

            case CheckpointType::Finish:
                col = {230, 40, 40, 255};  // rojo
                break;

            case CheckpointType::Intermediate:
            default:
                col = {240, 200, 40, 255};  // amarillo
                break;
        }

        // si es el siguiente → más fuerte
        if (cp.order == nextCP)
            col.a = 220;
        else
            col.a = 90;

        SDL_SetRenderDrawColor(r, col.r, col.g, col.b, col.a);

        // -----------------------------------------
        //    ROTACIÓN (usar la función auxiliar ExF)
        // -----------------------------------------

        drawRotatedRect(r, cp.x * PPM - camX, cp.y * PPM - camY, rw, rh,
                        cp.angle, col);

        // borde
        SDL_SetRenderDrawColor(r, col.r / 2, col.g / 2, col.b / 2, 255);
        SDL_RenderDrawRectF(r, &rect);
    }

    SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_NONE);
}

static void renderUI(SDL_Renderer* r, const CarSnapshot& carDyn,
                     const float maxSpeed) {
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
    float speedRatio = std::clamp(carDyn.speed / maxSpeed, 0.0f, 1.0f);
    SDL_FRect bgS = {uiX, uiY + 30.0f, 200.0f, 20.0f};
    SDL_FRect fgS = {uiX, uiY + 30.0f, 200.0f * speedRatio, 20.0f};
    SDL_SetRenderDrawColor(r, 60, 60, 60, 255);
    SDL_RenderFillRectF(r, &bgS);
    SDL_SetRenderDrawColor(r, 40, 180, 255, 255);
    SDL_RenderFillRectF(r, &fgS);
    SDL_SetRenderDrawColor(r, 255, 255, 255, 255);
    SDL_RenderDrawRectF(r, &bgS);
}

inline int test(const YamlGameConfig& cfg) {
    try {
        if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
            std::cerr << "SDL_Init error: " << SDL_GetError() << "\n";
            return 1;
        }
        if (TTF_Init() != 0) {
            std::cerr << "TTF_Init error: " << TTF_GetError() << "\n";
            return 1;
        }
        SDL_Window* window = SDL_CreateWindow(
            "Box2D Car Test", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
            WIN_W, WIN_H, SDL_WINDOW_SHOWN);
        SDL_Renderer* renderer =
            SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

        // DURANTE EL LOBBY DEBERIA GENERAR ESTO
        RaceDefinition race{"LibertyCity",
                            "assets/liberty_city_circuito1.yaml"};
        PlayerConfig player{1, "Tester", CarType::Speedster};
        PlayerConfig player2{2, "Ghost", CarType::Ghost};
        std::vector<RaceDefinition> races{race};
        std::vector<PlayerConfig> players{player, player2};

        GameSessionFacade game(cfg, races, players);
        auto stat = game.getStaticSnapshot();
        game.start();  // EMPEZAR PARTIDA EN EL LOBBY -> CAMBIAR A SDL

        bool running = true;

        TTF_Font* font = TTF_OpenFont("assets/fonts/OpenSans-Regular.ttf", 16);
        if (!font) {
            std::cerr << "No pude abrir fuente: " << TTF_GetError() << "\n";
            return 1;
        }
        SDL_Event e;
        while (running) {
            while (SDL_PollEvent(&e)) {
                if (e.type == SDL_QUIT) running = false;

                if (e.type == SDL_KEYDOWN && !e.key.repeat) {
                    switch (e.key.keysym.sym) {
                        case SDLK_ESCAPE:
                            running = false;
                            break;
                        case SDLK_w:
                            game.startAccelerating(1);
                            break;
                        case SDLK_s:
                            game.startReversing(1);
                            break;
                        case SDLK_a:
                            game.startTurningLeft(1);
                            break;
                        case SDLK_d:
                            game.startTurningRight(1);
                            break;
                        case SDLK_LSHIFT:
                        case SDLK_RSHIFT:
                            game.useNitro(1);
                            break;
                        case SDLK_SPACE:
                            game.startAccelerating(2);
                            break;
                    }
                }
                if (e.type == SDL_KEYUP && !e.key.repeat) {
                    switch (e.key.keysym.sym) {
                        case SDLK_w:
                            game.stopAccelerating(1);
                            break;
                        case SDLK_s:
                            game.stopReversing(1);
                            break;
                        case SDLK_a:
                            game.stopTurningLeft(1);
                            break;
                        case SDLK_d:
                            game.stopTurningRight(1);
                            break;
                        case SDLK_SPACE:
                            game.stopAccelerating(2);
                            break;
                    }
                }
            }

            // game.update(DT);
            auto dyn = game.getSnapshot();
            if (dyn.players.empty()) continue;

            // jugador local
            PlayerId localPlayerId = 1;
            auto itLocal = std::find_if(
                dyn.players.begin(), dyn.players.end(),
                [&](const PlayerSnapshot& p) { return p.id == localPlayerId; });
            if (itLocal == dyn.players.end()) continue;
            const auto& local = *itLocal;

            // Cámara centrada en jugador local
            float camX = local.car.x * PPM - WIN_W / 2.0f;
            float camY = local.car.y * PPM - WIN_H / 2.0f;

            // --- Render ---
            SDL_SetRenderDrawColor(renderer, 25, 25, 25, 255);
            SDL_RenderClear(renderer);

            // Fondo: walls + puentes (parte baja / sombra)
            renderBuildings(renderer, stat, camX, camY);
            renderBridgesBelowCars(renderer, stat, camX, camY);
            renderCheckpoints(renderer, stat, dyn.players, camX, camY);
            renderRailingBelowCars(renderer, stat, camX, camY);

            auto drawCar = [&](const PlayerSnapshot& ps) {
                const auto& carDyn = ps.car;
                const auto& carStat =
                    cfg.getCarStaticStatsMap().at(ps.car.type);

                float px = carDyn.x * PPM - camX;
                float py = carDyn.y * PPM - camY;

                SDL_FRect carRect{px - carStat.width * PPM * 0.5f,
                                  py - carStat.height * PPM * 0.5f,
                                  carStat.width * PPM, carStat.height * PPM};
                SDL_FPoint center{carRect.w / 2.0f, carRect.h / 2.0f};

                if (ps.id == localPlayerId) {
                    SDL_SetRenderDrawColor(renderer, 40, 180, 240,
                                           255);  // celeste
                } else {
                    SDL_SetRenderDrawColor(renderer, 255, 120, 50,
                                           255);  // naranja
                }

                SDL_RenderFillRectExF(
                    renderer, &carRect,
                    carDyn.angle * 180.0f / static_cast<float>(M_PI), &center,
                    SDL_FLIP_NONE);

                // Línea de dirección
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                float noseLen = carRect.h * 0.5f;
                float nx = px + std::cos(carDyn.angle) * noseLen;
                float ny = py + std::sin(carDyn.angle) * noseLen;
                SDL_RenderDrawLineF(renderer, px, py, nx, ny);
            };

            // 4) Autos capa UNDER
            for (const auto& ps : dyn.players) {
                if (ps.car.layer == RenderLayer::UNDER) {
                    drawCar(ps);
                }
            }

            renderBridgeDeckAboveUnderCars(renderer, stat, camX, camY);

            for (const auto& ps : dyn.players) {
                if (ps.car.layer == RenderLayer::OVER) {
                    drawCar(ps);
                }
            }
            renderOverpassesOnTop(renderer, stat, camX, camY);
            renderRailingAboveCars(renderer, stat, camX, camY);

            const auto& debugSensors = game.getDebugSensors();
            renderSensors(renderer, debugSensors, camX, camY);

            // ============================================================
            //  Hint direccional (flecha al siguiente checkpoint)
            // ============================================================
            if (!dyn.players.empty()) {
                const auto& locall = *itLocal;
                int nextCP = locall.raceProgress.nextCheckpoint;

                if (nextCP < static_cast<int>(stat.checkpoints.size())) {
                    const auto& cp = stat.checkpoints[nextCP];

                    // centro del auto
                    float px = locall.car.x * PPM - camX;
                    float py = locall.car.y * PPM - camY;

                    // vector hacia el checkpoint
                    float dx = (cp.x - locall.car.x);
                    float dy = (cp.y - locall.car.y);
                    float len = std::sqrt(dx * dx + dy * dy);
                    if (len > 0.001f) {
                        dx /= len;
                        dy /= len;
                    }

                    // frente del auto
                    float angle = locall.car.angle;
                    float fx = px + std::cos(angle) * 25;
                    float fy = py + std::sin(angle) * 25;

                    // línea principal (flecha)
                    SDL_SetRenderDrawColor(renderer, 255, 220, 0, 255);
                    SDL_RenderDrawLineF(renderer, fx, fy, fx + dx * 30,
                                        fy + dy * 30);

                    // cabeza triangular
                    SDL_FPoint tip = {fx + dx * 30, fy + dy * 30};
                    SDL_RenderDrawLineF(renderer, tip.x, tip.y, tip.x - dy * 5,
                                        tip.y + dx * 5);
                    SDL_RenderDrawLineF(renderer, tip.x, tip.y, tip.x + dy * 5,
                                        tip.y - dx * 5);
                }
            }

            auto localCarStat = cfg.getCarStaticStatsMap().at(local.car.type);

            renderUI(renderer, local.car, localCarStat.maxSpeed);
            renderMiniHUD(renderer, font, dyn);
            SDL_RenderPresent(renderer);
            SDL_Delay(static_cast<int>((DT * 1000)));
        }
        TTF_CloseFont(font);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        TTF_Quit();
        game.stop();
        return 0;
    } catch (const std::exception& e) {
        std::cerr << " Error durante las pruebas: " << e.what() << std::endl;
        return 1;
    }
}
#endif
