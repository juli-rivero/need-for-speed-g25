#include "client/game/game.h"

#include <SDL2/SDL.h>

#include <SDL2pp/SDL2pp.hh>

#include "common/timer_iterator.h"
#include "spdlog/spdlog.h"

Game::Game(SDL2pp::Renderer& renderer, SDL2pp::Mixer& mixer,
           Connexion& connexion, const GameSetUp& setup)
    : renderer(renderer),
      mixer(mixer),
      assets(renderer),
      city(*assets.city_name[setup.map]),
      api(connexion.get_api()),
      id(connexion.unique_id),
      players(setup.info.players) {
    renderer.Clear();
    Responder::subscribe(connexion);
}
Game::~Game() { Responder::unsubscribe(); }

//
// AUXILIAR
//
void Game::render(SDL2pp::Texture& texture, int x, int y, double angle,
                  bool in_world) {
    SDL2pp::Point pos(x, y);
    if (in_world) pos -= SDL2pp::Point(cam_x, cam_y);

    renderer.Copy(texture, SDL2pp::NullOpt, pos, angle);
}

void Game::render(const std::string& texto, int x, int y, bool in_world) {
    SDL2pp::Surface s =
        assets.font.RenderText_Solid(texto, SDL_Color{255, 255, 255, 255});
    SDL2pp::Texture t(renderer, s);

    render(t, x, y, 0, in_world);
}

void Game::render_rect(SDL2pp::Rect rect, const SDL2pp::Color& color,
                       bool in_world) {
    if (in_world) rect -= SDL2pp::Point(cam_x, cam_y);

    renderer.SetDrawColor(color).FillRect(rect);
}

//
// PRINCIPAL
//

void Game::on_game_snapshot(
    const float time_elapsed,
    const std::vector<PlayerSnapshot>& player_snapshots) {
    std::lock_guard lock(mutex);
    elapsed = time_elapsed;
    players = player_snapshots;
}

void Game::on_collision_event(const CollisionEvent& collision_event) {
    collisions.try_push(collision_event);
}

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
void Game::update_cars() {
    std::lock_guard lock(mutex);
    cars.clear();
    for (auto& player : players) {
        cars.emplace_back(*this, player);
        if (player.id == id) my_car = &cars.back();
    }
}

void Game::manage_collisions() {
    CollisionEvent collision;
    while (collisions.try_pop(collision)) {
        // TODO(franco): cambiar, dejo las herramientas que tenes como ejemplo
        std::visit(
            [](auto& collision) {
                spdlog::debug("collision on player {} with intensity {}",
                              collision.player, collision.intensity);
            },
            collision);
        if (auto* car_to_wall = std::get_if<CollisionSimple>(&collision)) {
            spdlog::debug("the player {} collide with a wall",
                          car_to_wall->player);
        }
        if (auto* car_to_car = std::get_if<CollisionCarToCar>(&collision)) {
            spdlog::debug("the player {} collide with player {}",
                          car_to_car->player, car_to_car->other);
        }
    }
}

void Game::get_state() {
    update_cars();
    manage_collisions();
}

void Game::draw_state() {
    renderer.Clear();

    // Ciudad
    spdlog::trace("cargando ciudad");
    render(assets.city_liberty, 0, 0);

    // Coches
    spdlog::trace("poniendo camara");
    if (my_car) my_car->set_camera();

    spdlog::trace("dibujando coches");
    for (Car& car : cars) {
        if (&car == my_car) continue;
        car.draw(true);
    }

    if (my_car) my_car->draw(false);

    // HUD
    spdlog::trace("mostrando HUD");
    // render("Hola, mundo!", 10, 10, false);
    if (my_car) {
        render_rect({10, 10, static_cast<int>(my_car->get_health()), 10},
                    {0, 255, 0, 255}, false);
        render_rect({10, 30, static_cast<int>(my_car->get_speed()), 10},
                    {255, 165, 0, 255}, false);
    }

    renderer.SetDrawColor(0, 0, 0, 255);
    renderer.Present();
}

void Game::play_sounds() {
    // for (Car& car : cars) {
    //     if (car.get_id() == 1) car.sound_crash();
    // }
}

bool Game::start() {
    constexpr std::chrono::duration<double> dt(1.f / 60.f);
    TimerIterator iterator(dt);

    while (1) {
        bool quit = send_events();
        get_state();
        draw_state();
        play_sounds();

        if (quit) return true;

        iterator.next();
    }
}
