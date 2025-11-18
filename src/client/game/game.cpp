#include "client/game/game.h"

#include <SDL2/SDL.h>

#include <SDL2pp/SDL2pp.hh>

Game::Game(SDL2pp::Renderer& renderer, SDL2pp::Mixer& mixer)
    : renderer(renderer), mixer(mixer), assets(renderer) {
    renderer.Clear();
}

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

#define MY_ID 0

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

void Game::get_state() {
    cars.clear();

    for (auto& player : api.get_snapshot().players) {
        cars.emplace_back(*this, player);
        if (player.id == MY_ID) my_car = &cars.back();
    }
}

void Game::draw_state() {
    renderer.Clear();

    // Ciudad
    render(assets.city_liberty, 0, 0);

    // Coches
    my_car->set_camera();
    for (Car& car : cars) {
        if (&car == my_car) continue;
        car.draw(true);
    }

    my_car->draw(false);

    // HUD
    // render("Hola, mundo!", 10, 10, false);
    render_rect({10, 10, static_cast<int>(my_car->get_health()), 10},
                {0, 255, 0, 255}, false);
    render_rect({10, 30, static_cast<int>(my_car->get_speed()), 10},
                {255, 165, 0, 255}, false);

    renderer.SetDrawColor(0, 0, 0, 255);
    renderer.Present();
}

void Game::play_sounds() {
    // for (Car& car : cars) {
    //     if (car.get_id() == 1) car.sound_crash();
    // }
}

void Game::wait_next_frame() {
    // TODO(crook): hacer 60 FPS verdaderos.
    SDL_Delay(1000 / 60);
}

bool Game::start() {
    while (1) {
        frame += 1;

        bool quit = send_events();
        get_state();
        draw_state();
        play_sounds();

        if (quit) {
            return true;
        } else {
            wait_next_frame();
        }
    }
}
