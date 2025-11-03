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

            if (tecla == SDLK_q || tecla == SDLK_ESCAPE) return true;

            if (tecla == SDLK_LEFT) left_held = true;
            if (tecla == SDLK_RIGHT) right_held = true;
            if (tecla == SDLK_UP) car_speed -= 1;
            if (tecla == SDLK_DOWN) car_speed += 1;
        }

        if (event.type == SDL_KEYUP) {
            auto tecla = event.key.keysym.sym;

            if (tecla == SDLK_LEFT) left_held = false;
            if (tecla == SDLK_RIGHT) right_held = false;
        }
    }

    return false;
}

// TODO(crook): temporal, hasta conectar con el servidor.
void Game::get_state() {
    if (left_held) car_angle -= 3;
    if (right_held) car_angle += 3;

    if (car_angle > 360) car_angle -= 360;
    if (car_angle < 0) car_angle += 360;
    car_x += sin(-car_angle * 3.14 / 180) * car_speed;
    car_y += cos(-car_angle * 3.14 / 180) * car_speed;
}

void Game::draw_state() {
    renderer.Clear();

    // Definir la camara alrededor del jugador.
    int cam_x =
        car_x + assets.car1.GetWidth() / 2 - renderer.GetOutputWidth() / 2;
    int cam_y =
        car_y + assets.car1.GetHeight() / 2 - renderer.GetOutputHeight() / 2;

    // Background
    renderer.Copy(assets.city_liberty, SDL2pp::NullOpt,
                  SDL2pp::Point(0 - cam_x, 0 - cam_y));

    // Foreground
    renderer.Copy(assets.car1, SDL2pp::NullOpt,
                  SDL2pp::Point(car_x - cam_x, car_y - cam_y), car_angle);

    // HUD
    SDL2pp::Texture text_render(
        renderer,
        assets.font.RenderText_Solid(hud_text, SDL_Color{255, 255, 255, 255}));
    renderer.Copy(text_render, SDL2pp::NullOpt, SDL2pp::Point(10, 10));

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
