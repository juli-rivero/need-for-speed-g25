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
    double car_x = 0;
    double car_y = 0;
    double car_speed = 0;
    double car_angle = 0;

    bool left_held = false;
    bool right_held = false;

    std::string hud_text = "Hello, world!";

    // Metodos de actualizacion internos
    bool send_events();
    void get_state();
    void draw_state();
    void wait_next_frame();

   public:
    explicit Game(SDL2pp::Renderer& renderer);

    bool start();

    MAKE_FIXED(Game)
};
