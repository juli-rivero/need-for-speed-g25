#pragma once

#include <SDL2pp/SDL2pp.hh>
#include <string>

#include "client/game/assets.h"
#include "common/macros.h"

class Game final {
   private:
    // Componentes graficos
    SDL2pp::Renderer& renderer;
    Assets assets;

    // Componentes logicos
    // TODO(crook): Temporal
    bool left_held = false;
    bool right_held = false;

    double car_x[7] = {0};
    double car_y[7] = {0};
    double car_speed[7] = {0};
    double car_angle[7] = {0};

    // Obviamente arrays de referencias no son validas en C++
    // Porque cuando C++ te hace la vida facil el planeta explotara.
    SDL2pp::Texture* car_sprite[7];

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
    explicit Game(SDL2pp::Renderer& renderer);

    bool start();

    MAKE_FIXED(Game)
};
