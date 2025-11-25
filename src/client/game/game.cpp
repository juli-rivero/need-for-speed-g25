#include <SDL2/SDL.h>

#include <SDL2pp/SDL2pp.hh>

#include "client/game/classes.h"
#include "common/timer_iterator.h"
#include "spdlog/spdlog.h"

Game::Game(SDL2pp::Renderer& renderer, SDL2pp::Mixer& mixer,
           Connexion& connexion, const GameSetUp& setup)
    : screen(renderer, *this),
      mixer(mixer),
      assets(renderer),
      city(*assets.city_name.at(setup.map)),
      api(connexion.get_api()),
      my_id(connexion.unique_id),
      setup(setup) {
    player_snapshots = setup.info.players;

    spdlog::info("map: {}", setup.map);
    for (const CheckpointInfo& c : setup.info.checkpoints) {
        spdlog::info("check: x {} | y {} | w {} | h {}", c.x, c.y, c.w, c.h);
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

void Game::manage_collisions() {
    CollisionEvent collision;
    while (collisions.try_pop(collision)) {
        PlayerId id;
        std::visit([&](const auto& collision) { id = collision.player; },
                   collision);

        Car& car = cars.at(id);
        car.sound_crash();

        spdlog::debug("choque con {}", id);
    }
}

bool Game::start() {
    constexpr std::chrono::duration<double> dt(1.f / 60.f);
    TimerIterator iterator(dt);

    while (1) {
        bool quit = send_events();
        update_state();
        manage_collisions();

        screen.update();

        if (quit) return true;

        iterator.next();
    }
}
