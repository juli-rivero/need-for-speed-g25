#include "client/game/sdl_app.h"

SdlApp::SdlApp(ConnexionController& connexion_controller, bool& quit)
    : connexion_controller(connexion_controller),
      input_handler(connexion_controller) {
    connexion_controller.control(game);

    Game game;
    quit = game.start();
}

SdlApp::~SdlApp() { connexion_controller.decontrol(game); }
