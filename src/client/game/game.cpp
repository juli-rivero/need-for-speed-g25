#include <SDL2/SDL.h>

#include <SDL2pp/SDL2pp.hh>

#include "client/game/classes.h"
#include "common/timer_iterator.h"
#include "spdlog/spdlog.h"

Game::Game(SDL2pp::Renderer& renderer, SDL2pp::Mixer& mixer,
           Connexion& connexion, const GameSetUp& setup)
    : screen(renderer, *this),
      sound(mixer, *this),
      assets(renderer),
      api(connexion.get_api()),
      my_id(connexion.unique_id),
      city_info(setup.city_info),
      race_info(setup.race_info) {
    spdlog::info("map: {}", city_info.name);
    for (const CheckpointInfo& c : race_info.checkpoints) {
        const Bound& cb = c.bound;
        spdlog::info("check: x {} | y {} | w {} | h {}", cb.pos.x, cb.pos.y,
                     cb.width, cb.height);
    }

    Responder::subscribe(connexion);
}
Game::~Game() { Responder::unsubscribe(); }

//
// DATOS SERVIDOR
//
void Game::on_game_snapshot(
    const float time_elapsed,
    const std::vector<PlayerSnapshot>& player_snapshots) {
    std::lock_guard lock(snapshot_mutex);

    this->time_elapsed = time_elapsed;
    this->player_snapshots = player_snapshots;
}

void Game::on_collision_event(const CollisionEvent& collision_event) {
    collisions.try_push(collision_event);
}

//
// PRINCIPAL
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
    for (auto& player : player_snapshots) {
        cars.emplace(std::piecewise_construct, std::forward_as_tuple(player.id),
                     std::forward_as_tuple(*this, player));
        if (player.id == my_id) my_car = &cars.at(player.id);
    }
}

void Game::manage_sounds() {
    // Colisiones
    CollisionEvent collision;
    while (collisions.try_pop(collision)) {
        PlayerId id;
        std::visit([&](const auto& collision) { id = collision.player; },
                   collision);

        Car& car = cars.at(id);
        sound.crash(car);
    }

    // Freno (y carrera completada)
    // TODO(franco): reproducir frenos de otros coches, aparte del mio
    if (!my_car) return;
    sound.test_brake(*my_car);
    sound.test_finish(*my_car);
}

bool Game::start() {
    constexpr std::chrono::duration<double> dt(1.f / 60.f);
    TimerIterator iterator(dt);

    while (1) {
        bool quit = send_events();
        update_state();
        screen.update();
        manage_sounds();

        if (quit) return true;

        iterator.next();
    }
}
