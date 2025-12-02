#include "client/game/game.h"

#include <SDL2/SDL.h>

#include <SDL2pp/SDL2pp.hh>

#include "common/timer_iterator.h"
#include "spdlog/spdlog.h"

//
// INICIALIZACION
//
Game::Game(SDL2pp::Renderer& renderer, SDL2pp::Mixer& mixer,
           Connexion& connexion, const GameSetUp& setup)
    : api(connexion.get_api()),
      city_info(setup.city_info),
      race_info(setup.race_info),
      screen(renderer, *this, setup.city_info.name),
      sound(mixer, *this, setup.city_info.name),
      my_id(connexion.unique_id) {
    // Configurar componentes de snapshot estatico
    for (const Bound& b : city_info.walls) {
        walls.emplace_back(b);
    }
    for (const Bound& b : city_info.bridges) {
        bridges.emplace_back(b);
    }
    for (const Bound& b : city_info.railings) {
        bridges.emplace_back(b);
    }
    for (const Bound& b : city_info.overpasses) {
        overpasses.emplace_back(b);
    }
    for (const CheckpointInfo& ci : race_info.checkpoints) {
        checkpoints.emplace_back(ci.bound, ci.angle);
    }
    checkpoint_amount = checkpoints.size();

    Responder::subscribe(connexion);
}
Game::~Game() { Responder::unsubscribe(); }

//
// DATOS SERVIDOR
//
void Game::on_game_snapshot(const GameSnapshot& game_snapshot) {
    std::lock_guard lock(snapshot_mutex);

    current_snapshot = game_snapshot;
}

void Game::on_collision_event(const CollisionEvent& collision_event) {
    collisions.try_push(collision_event);
}

//
// CAMARA
//
void Game::set_camera(const PlayerCar& car) {
    SDL2pp::Point center = car.pos.get_center();
    cam_x = center.GetX();
    cam_y = center.GetY();
}

//
// PASOS ACTUALIZACION
//
bool Game::send_events() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) return true;

        if (event.type == SDL_KEYDOWN) {
            auto tecla = event.key.keysym.sym;

            if (tecla == SDLK_ESCAPE) return true;

            if (tecla == SDLK_LEFT || tecla == SDLK_a)
                api.start_turning(TurnDirection::Left);
            if (tecla == SDLK_RIGHT || tecla == SDLK_d)
                api.start_turning(TurnDirection::Right);
            if (tecla == SDLK_UP || tecla == SDLK_w) api.start_accelerating();
            if (tecla == SDLK_DOWN || tecla == SDLK_s) api.start_breaking();

            if (tecla == SDLK_SPACE) api.start_using_nitro();

            if (tecla == SDLK_RALT) cheat_mode = true;
            if (cheat_mode && tecla == SDLK_v) {
                api.cheat(Cheat::InfiniteHealth);
                cheat_used = true;
            }
            if (cheat_mode && tecla == SDLK_g) {
                api.cheat(Cheat::FinishRace);
                cheat_used = true;
            }
            if (cheat_mode && tecla == SDLK_p) {
                api.cheat(Cheat::DestroyAllCars);
                cheat_used = true;
            }

            if (match_state == MatchState::Intermission) {
                if (tecla == SDLK_1) api.upgrade(UpgradeStat::Acceleration);
                if (tecla == SDLK_2) api.upgrade(UpgradeStat::MaxSpeed);
                if (tecla == SDLK_3) api.upgrade(UpgradeStat::Nitro);
                if (tecla == SDLK_4) api.upgrade(UpgradeStat::Health);
            }
        }

        if (event.type == SDL_KEYUP) {
            auto tecla = event.key.keysym.sym;

            if (tecla == SDLK_LEFT || tecla == SDLK_a)
                api.stop_turning(TurnDirection::Left);
            if (tecla == SDLK_RIGHT || tecla == SDLK_d)
                api.stop_turning(TurnDirection::Right);
            if (tecla == SDLK_UP || tecla == SDLK_w) api.stop_accelerating();
            if (tecla == SDLK_DOWN || tecla == SDLK_s) api.stop_breaking();

            if (tecla == SDLK_RALT) cheat_mode = false;
        }
    }

    return false;
}

void Game::update_state() {
    std::lock_guard lock(snapshot_mutex);

    // Estado para eventos de sonido
    if (my_car != nullptr) {
        old_checkpoint = my_car->next_checkpoint;
        old_finished = my_car->finished;
    }
    for (auto& [id, car] : cars) {
        old_braking[id] = car.braking;
        old_disqualified[id] = car.disqualified;
        old_nitro_active[id] = car.nitro_active;
    }

    // Sumado/reemplazo de jugadores y NPCs
    cars.clear();
    for (auto& player : current_snapshot.players) {
        cars.emplace(player.id, player);
        if (player.id == my_id) {
            my_car = &cars.at(player.id);
            my_upgrades = player.upgrades;
        }
    }

    npcs.clear();
    for (auto& npc : current_snapshot.npcs) {
        npcs.emplace_back(npc);
    }

    // Tiempo y estado de carrera
    time_elapsed = current_snapshot.race.raceElapsed;
    time_countdown = current_snapshot.race.raceCountdown;
    time_left = current_snapshot.race.raceTimeLeft;
    race_state = current_snapshot.race.raceState;
    match_state = current_snapshot.match.matchState;
}

//
// PRINCIPAL
//
bool Game::start() {
    constexpr std::chrono::duration<double> dt(1.f / 60.f);
    TimerIterator iterator(dt);

    while (1) {
        bool quit = send_events();
        update_state();

        if (my_car) set_camera(*my_car);
        screen.update();
        sound.update();

        if (quit) return true;

        iterator.next();
    }
}
