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
      city(*assets.city_name.at(setup.map)),
      api(connexion.get_api()),
      my_id(connexion.unique_id),
      setup(setup) {
    player_snapshots = setup.info.players;

    spdlog::info("map: {}", setup.map);
    for (const CheckpointInfo& c : setup.info.checkpoints) {
        spdlog::info("check: x {} | y {} | w {} | h {}", c.x, c.y, c.w, c.h);
    }

    renderer.Clear();
    renderer.SetDrawBlendMode(SDL_BLENDMODE_BLEND);

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
        cars.emplace_back(*this, player);
        if (player.id == my_id) my_car = &cars.back();
    }
}

void Game::manage_collisions() {
    CollisionEvent collision;
    while (collisions.try_pop(collision)) {
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

static std::string format_time(float time) {
    std::ostringstream ss;
    ss << std::fixed << std::setprecision(0);

    // Si, esto definitivamente es mejor que una llamada a sprintf.
    // Definitivamente.
    int time_i = static_cast<int>(time);
    ss << time_i / 60 << ":";
    ss << std::setw(2) << std::setfill('0') << time_i % 60 << ".";
    ss << std::setw(2) << std::setfill('0') << fmod(time, 1) * 100;
    return ss.str();
}

void Game::draw_state() {
    renderer.Clear();
    if (my_car) my_car->set_camera();

    // Ciudad
    render(assets.city_liberty, 0, 0);

    // Checkpoints
    if (my_car &&
        my_car->get_next_checkpoint() < setup.info.checkpoints.size()) {
        const CheckpointInfo& c =
            setup.info.checkpoints[my_car->get_next_checkpoint()];
        render_rect({static_cast<int>(c.x * 10), static_cast<int>(c.y * 10),
                     static_cast<int>(c.w * 10), static_cast<int>(c.h * 10)},
                    {0, 255, 0, 128});
    }

    // Coches
    for (Car& car : cars) {
        if (&car == my_car) continue;
        car.draw(true);
    }

    if (my_car) my_car->draw(false);

    // HUD
    render(format_time(time_elapsed), 10, 50, false);

    if (my_car) {
        render_rect({10, 10, static_cast<int>(my_car->get_health()), 10},
                    {0, 255, 0, 255}, false);
        render_rect({10, 30, static_cast<int>(my_car->get_speed()), 10},
                    {255, 165, 0, 255}, false);

        bool has_angle = false;
        float angle = my_car->get_angle_to_next_checkpoint(has_angle);
        if (has_angle) {
            // TODO(franco): no hardcodear constantes, crearlas
            render(assets.arrow, 320 - 32, 10, angle, false);
        }
    }

    renderer.SetDrawColor(0, 0, 0, 255);
    renderer.Present();
}

bool Game::start() {
    constexpr std::chrono::duration<double> dt(1.f / 60.f);
    TimerIterator iterator(dt);

    while (1) {
        bool quit = send_events();
        update_state();
        manage_collisions();
        draw_state();

        if (quit) return true;

        iterator.next();
    }
}
