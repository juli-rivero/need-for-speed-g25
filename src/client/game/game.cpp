#include "client/game/game.h"

#include <SDL2/SDL.h>

#include <SDL2pp/SDL2pp.hh>

Game::Game(SDL2pp::Renderer& renderer) : renderer(renderer), assets(renderer) {}

// TODO(crook): temporal, hasta conectar con el servidor.
bool Game::send_events() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) return true;

        if (event.type == SDL_KEYDOWN) {
            auto tecla = event.key.keysym.sym;

            if (tecla == SDLK_q) return true;

            if (tecla == SDLK_LEFT) car_angle -= 10;
            if (tecla == SDLK_RIGHT) car_angle += 10;
            if (tecla == SDLK_UP) car_speed -= 1;
            if (tecla == SDLK_DOWN) car_speed += 1;
        }
    }

    return false;
}

// TODO(crook): temporal, hasta conectar con el servidor.
void Game::get_state() {
    if (car_angle > 360) car_angle -= 360;
    if (car_angle < 0) car_angle += 360;
    car_x += sin(-car_angle * 3.14 / 180) * car_speed;
    car_y += cos(-car_angle * 3.14 / 180) * car_speed;
}

void Game::draw_state() {
    renderer.Clear();
    renderer.Copy(assets.city_liberty, SDL2pp::NullOpt, SDL2pp::Point(0, 0));
    renderer.Copy(assets.car1, SDL2pp::NullOpt, SDL2pp::Point(car_x, car_y),
                  car_angle);
    renderer.Present();
}

void Game::wait_next_frame() {
    // TODO(crook): hacer 60 FPS verdaderos.
    SDL_Delay(1000 / 60);
}

bool Game::start() {
    while (1) {
        bool quit = send_events();
        get_state();
        draw_state();

        if (quit) {
            return true;
        } else {
            wait_next_frame();
        }
    }
}
