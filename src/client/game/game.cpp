#include "client/game/game.h"

#include <SDL2/SDL.h>

#include <SDL2pp/SDL2pp.hh>

Game::Game(SDL2pp::Renderer& renderer, SDL2pp::Mixer& mixer)
    : renderer(renderer), mixer(mixer), assets(renderer) {
    renderer.Clear();

    // Coche 0: yo
    // Coche 1: simulado
    for (size_t i = 0; i < 7; ++i) {
        SDL2pp::Texture* texture;

        if (i == 0) texture = &assets.car1;
        if (i == 1) texture = &assets.car2;
        if (i == 2) texture = &assets.car3;
        if (i == 3) texture = &assets.car4;
        if (i == 4) texture = &assets.car5;
        if (i == 5) texture = &assets.car6;
        if (i == 6) texture = &assets.car7;

        cars.push_back(Car(*this, 100 * i, 100 * i, 3, 0, *texture, i));
    }
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
    space_pressed = false;

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
                up_held = true;  // EVENT: Enviar "iniciar acelerar"
            if (tecla == SDLK_DOWN)
                down_held = true;  // EVENT: Enviar "iniciar desacelerar"

            if (tecla == SDLK_SPACE)
                space_pressed = true;  // EVENT: Enviar "activar nitro"
        }

        if (event.type == SDL_KEYUP) {
            auto tecla = event.key.keysym.sym;

            if (tecla == SDLK_LEFT)
                left_held = false;  // EVENT: Enviar "detener giro izquierda"
            if (tecla == SDLK_RIGHT)
                right_held = false;  // EVENT: Enviar "detener giro derecha"
            if (tecla == SDLK_UP)
                up_held = false;  // EVENT: Enviar "detener acelerar"
            if (tecla == SDLK_DOWN)
                down_held = false;  // EVENT: Enviar "detener desacelerar"
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

    for (Car& car : cars) car.update();
}

void Game::draw_state() {
    renderer.Clear();

    // Ciudad
    render(assets.city_liberty, 0, 0);

    // Coches
    for (Car& car : cars) {
        if (car.get_id() == 0) car.set_camera();
        car.draw();
    }

    // HUD
    render("Hola, mundo!", 10, 10, false);

    renderer.Present();
}

void Game::play_sounds() {
    for (Car& car : cars) {
        if (car.get_id() == 1) car.sound_crash();
    }
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
