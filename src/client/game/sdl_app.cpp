#include "client/game/sdl_app.h"

// FIXME(crook): Unificar sdl_app.cpp y game.cpp?

SdlApp::SdlApp(ConnexionController& connexion_controller, bool& quit)
    : connexion_controller(connexion_controller),
      input_handler(connexion_controller) {
    // Inicializacion de SDL (ventana 640x480)
    SDL2pp::SDL sdl(SDL_INIT_VIDEO);
    SDL2pp::Window window("Need for Speed TPG", SDL_WINDOWPOS_UNDEFINED,
                          SDL_WINDOWPOS_UNDEFINED, 640, 480, 0);
    SDL2pp::Renderer renderer(window, -1, SDL_RENDERER_ACCELERATED);

    // Iniciar partida
    Game game(renderer);
    // connexion_controller.control(game);
    quit = game.start();

    (void)connexion_controller;
}

SdlApp::~SdlApp() {
    // connexion_controller.decontrol(game);
}
