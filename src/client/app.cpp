#include "client/app.h"

#include "client/game/sdl_app.h"
#include "client/qt/qt_app.h"

App::App(const std::string& host, const std::string& port)
    : connexion_controller(Protocol(host.c_str(), port.c_str())) {}

void App::run() {
    bool quit = false;
    while (true) {
        QtApp(connexion_controller, quit);
        if (quit) return;
        SdlApp(connexion_controller, quit);
        if (quit) return;
    }
}
