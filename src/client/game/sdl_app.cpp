#include "client/game/sdl_app.h"

SdlApp::SdlApp(ConnexionController& connexion_controller, bool& quit)
    : connexion_controller(connexion_controller),
      input_handler(connexion_controller) {
    connexion_controller.control(game);

    // TODO(Franco): borrar cuando esté el quit = true en algun lado, lo pongo
    // para que el linter no moleste
    quit = false;
}

SdlApp::~SdlApp() { connexion_controller.decontrol(game); }
