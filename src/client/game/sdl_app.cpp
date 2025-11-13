#include "client/game/sdl_app.h"

#include "client/game/game.h"

SdlApp::SdlApp(Connexion& connexion, bool& quit)
    : connexion(connexion), input_handler(connexion) {
    // Inicializacion de SDL (ventana 640x480)
    SDL2pp::SDL sdl(SDL_INIT_VIDEO);
    SDL2pp::Window window("Need for Speed TPG", SDL_WINDOWPOS_UNDEFINED,
                          SDL_WINDOWPOS_UNDEFINED, 640, 480, 0);
    SDL2pp::Renderer renderer(window, -1, SDL_RENDERER_ACCELERATED);

    SDL2pp::SDLTTF sdl_ttf;

    SDL2pp::Mixer mixer(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT,
                        MIX_DEFAULT_CHANNELS, 4096);

    // Iniciar partida
    Game game(renderer, mixer);
    // connexion.control(game);
    quit = game.start();

    (void)this->connexion;
}

SdlApp::~SdlApp() {
    // connexion.decontrol(game);
}
