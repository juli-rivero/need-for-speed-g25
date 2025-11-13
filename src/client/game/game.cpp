#include "client/game/game.h"

#include <SDL2/SDL.h>

#include <SDL2pp/SDL2pp.hh>

Game::Game(SDL2pp::Renderer& renderer, SDL2pp::Mixer& mixer)
    : renderer(renderer), mixer(mixer), assets(renderer) {
    renderer.Clear();

    // Coche 0: yo
    // Coche 1: simulado
    for (size_t i = 1; i < 7; ++i) {
        car_x[i] = 100 * i;
        car_y[i] = 100 * i;
        car_speed[i] = 3;
        // car_angle[i] = 0;
    }

    car_sprite[0] = &assets.car1;
    car_sprite[1] = &assets.car2;
    car_sprite[2] = &assets.car3;
    car_sprite[3] = &assets.car4;
    car_sprite[4] = &assets.car5;
    car_sprite[5] = &assets.car6;
    car_sprite[6] = &assets.car7;
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

            if (tecla == SDLK_LEFT)
                left_held = true;  // EVENT: Enviar "iniciar giro izquierda"
            if (tecla == SDLK_RIGHT)
                right_held = true;  // EVENT: Enviar "iniciar giro derecha"
            if (tecla == SDLK_UP)
                car_speed[0] -= 1;  // EVENT: Enviar "iniciar acelerar"
            if (tecla == SDLK_DOWN)
                car_speed[0] += 1;  // EVENT: Enviar "iniciar desacelerar"

            // if (tecla == SDLK_SPACE) // EVENT: Enviar "activar nitro

            // Pruebas de sonido, se removeran despues
            if (tecla == SDLK_c) mixer.PlayChannel(-1, assets.sound_crash);
        }

        if (event.type == SDL_KEYUP) {
            auto tecla = event.key.keysym.sym;

            if (tecla == SDLK_LEFT)
                left_held = false;  // EVENT: Enviar "detener giro izquierda"
            if (tecla == SDLK_RIGHT)
                right_held = false;  // EVENT: Enviar "detener giro derecha"
            // if (tecla == SDLK_UP) // EVENT: Enviar "detener acelerar"
            // if (tecla == SDLK_DOWN) // EVENT: Enviar "detener desacelerar"
        }
    }

    return false;
}

void Game::get_state() {
    // EVENT: Aca en teoria es donde actualizaria mi estado interno en base al
    // snapshot del servidor.
    //        por ahora calculo todo localmente, como una clase de "simulacion",
    //        para tener algo para dibujar. Si es necesario borrar esto, despues
    //        lo vemos.

    if (left_held) car_angle[0] -= 3;
    if (right_held) car_angle[0] += 3;

    for (size_t i = 1; i < 7; ++i) car_angle[i] += 3;

    for (size_t i = 0; i < 7; ++i) {
        if (car_angle[i] > 360) car_angle[i] -= 360;
        if (car_angle[i] < 0) car_angle[i] += 360;
        car_x[i] += sin(-car_angle[i] * 3.14 / 180) * car_speed[i];
        car_y[i] += cos(-car_angle[i] * 3.14 / 180) * car_speed[i];
    }
}

void Game::draw_state() {
    renderer.Clear();

    // Definir la camara alrededor del jugador.
    cam_x =
        car_x[0] + assets.car1.GetWidth() / 2 - renderer.GetOutputWidth() / 2;
    cam_y =
        car_y[0] + assets.car1.GetHeight() / 2 - renderer.GetOutputHeight() / 2;

    // Ciudad
    render(assets.city_liberty, 0, 0);

    // Coches
    for (size_t i = 0; i < 7; ++i)
        render(*car_sprite[i], car_x[i], car_y[i], car_angle[i]);

    // HUD
    render("Hola, mundo!", 10, 10, false);

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
