#include <SDL2/SDL.h>

#include <SDL2pp/SDL2pp.hh>

#include "client/game/classes.h"
#include "common/timer_iterator.h"
#include "spdlog/spdlog.h"

//
// INICIALIZACION
//
Game::Game(SDL2pp::Renderer& renderer, SDL2pp::Mixer& mixer,
           Connexion& connexion, const GameSetUp& setup)
    : api(connexion.get_api()),
      my_id(connexion.unique_id),
      screen(renderer, *this),
      sound(mixer, *this),
      assets(renderer),
      city_info(setup.city_info),
      race_info(setup.race_info) {
    // Configurar componentes de snapshot estatico
    for (const Bound& b : city_info.bridges) {
        spdlog::info("bridge: {} {} {} {}", b.pos.x, b.pos.y, b.width,
                     b.height);
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

    spdlog::info("map: {}", city_info.name);
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

            if (tecla == SDLK_q || tecla == SDLK_ESCAPE) return true;

            if (tecla == SDLK_LEFT || tecla == SDLK_a)
                api.start_turning(TurnDirection::Left);
            if (tecla == SDLK_RIGHT || tecla == SDLK_d)
                api.start_turning(TurnDirection::Right);
            if (tecla == SDLK_UP || tecla == SDLK_w) api.start_accelerating();
            if (tecla == SDLK_DOWN || tecla == SDLK_s) api.start_breaking();

            if (tecla == SDLK_SPACE) api.start_using_nitro();
        }

        if (event.type == SDL_KEYUP) {
            auto tecla = event.key.keysym.sym;

            if (tecla == SDLK_LEFT || tecla == SDLK_a)
                api.stop_turning(TurnDirection::Left);
            if (tecla == SDLK_RIGHT || tecla == SDLK_d)
                api.stop_turning(TurnDirection::Right);
            if (tecla == SDLK_UP || tecla == SDLK_w) api.stop_accelerating();
            if (tecla == SDLK_DOWN || tecla == SDLK_s) api.stop_breaking();
        }
    }

    return false;
}
void Game::update_state() {
    std::lock_guard lock(snapshot_mutex);

    cars.clear();
    for (auto& player : current_snapshot.players) {
        cars.emplace(player.id, player);
        if (player.id == my_id) my_car = &cars.at(player.id);
    }

    npcs.clear();
    for (auto& npc : current_snapshot.npcs) {
        npcs.emplace_back(npc);
    }

    time_elapsed = current_snapshot.race.raceElapsed;
    time_countdown = current_snapshot.race.raceCountdown;
    time_left = current_snapshot.race.raceTimeLeft;
}

void Game::manage_sounds() {
    // Colisiones
    CollisionEvent collision;
    while (collisions.try_pop(collision)) {
        PlayerId id;
        std::visit([&](const auto& collision) { id = collision.player; },
                   collision);

        PlayerCar& car = cars.at(id);
        sound.crash(car);
    }

    // Freno (y carrera completada)
    // TODO(franco): reproducir frenos de otros coches, aparte del mio
    if (!my_car) return;
    sound.test_brake(*my_car);
    sound.test_finish(*my_car);
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
        manage_sounds();

        if (quit) return true;

        iterator.next();
    }
}
