#include "client/app.h"

#include <assert.h>

#include <memory>

#include "client/connexion/connexion_controller.h"
#include "client/game/sdl_app.h"
#include "client/qt/qt_app.h"

void App::run() {
    std::unique_ptr<ConnexionController> connexion_controller;
    bool quit = false;
    while (true) {
        QtApp(connexion_controller, quit);
        if (quit) return;
        assert(connexion_controller != nullptr);
        SdlApp(*connexion_controller, quit);
        if (quit) return;
    }
}
