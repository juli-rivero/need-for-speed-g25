#include "client/game/sdl_app.h"

SdlApp::SdlApp(Connexion& connexion, bool& quit)
    : connexion(connexion), input_handler(connexion) {
    connexion.control(game);

    // TODO(Franco): borrar cuando esté el quit = true en algun lado, lo pongo
    // para que el linter no moleste
    quit = false;
}

SdlApp::~SdlApp() { connexion.decontrol(game); }
