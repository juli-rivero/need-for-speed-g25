#include "client/app.h"

#include "client/game/sdl_app.h"
#include "client/qt/qt_app.h"
#include "spdlog/spdlog.h"

App::App(const std::string& host, const std::string& port)
    : connexion(Protocol(host.c_str(), port.c_str())) {
    spdlog::trace("app created");
}

App::~App() { spdlog::trace("app destroyed"); }

void App::run() {
    bool quit = false;
    while (true) {
        QtApp(connexion, quit);
        if (quit) return;
        spdlog::trace("qt app closed");
        SdlApp(connexion, quit);
        if (quit) return;
        spdlog::trace("sdl app closed");
    }
}
