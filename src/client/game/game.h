#pragma once

#include <SDL2pp/SDL2pp.hh>
#include <list>
#include <string>

#include "client/game/assets.h"
#include "client/game/car.h"
#include "common/macros.h"

class Car;

class Game final {
    friend class Car;

   private:
    // Componentes graficos
    SDL2pp::Renderer& renderer;
    SDL2pp::Mixer& mixer;
    Assets assets;

    // Componentes logicos
    // TODO(crook): Estado de teclas temporal, se tienen que enviar eventos
    bool left_held = false;
    bool right_held = false;
    bool up_held = false;
    bool down_held = false;
    bool space_pressed = true;

    std::list<Car> cars;

    // Metodos de actualizacion internos
    bool send_events();
    void get_state();
    void draw_state();
    void wait_next_frame();

    // Auxiliar: dibujar una textura (o texto) en la ubicacion dada.
    // Con in_world == true, se dibuja relativo al mundo, no relativo a la
    // pantalla En ese caso, modificar las coordenadas de la camara para
    // impactar el punto base usado.
    int cam_x = 0;
    int cam_y = 0;

    void render(SDL2pp::Texture& surface, int x, int y, double angle = 0,
                bool in_world = true);
    void render(const std::string& texto, int x, int y, bool in_world = true);

   public:
    explicit Game(SDL2pp::Renderer& renderer, SDL2pp::Mixer& mixer);

    bool start();

    MAKE_FIXED(Game)
};
