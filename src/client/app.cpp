#include "client/app.h"

#include "client/game/sdl_app.h"
#include "client/qt/qt_app.h"

App::App(const char* host, const char* port)
    : connexion_controller(Protocol(host, port)) {}

void App::run() {
    bool quit = false;
    while (true) {
        QtApp(connexion_controller, quit);
        if (quit) return;
        SdlApp(connexion_controller, quit);
        if (quit) return;
    }
}
