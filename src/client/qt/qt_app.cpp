#include "client/qt/qt_app.h"

#include <assert.h>

#include <utility>

QtApp::QtApp(std::unique_ptr<ConnexionController>& connexion_controller,
             bool& quit)
    : connexion_controller(connexion_controller) {
    // TODO(Franco):
    // esto simula un evento donde se pasa del menu inicial al menu
    // browser. Hacer que el cliente se cree con el servidor y puertos pasados
    // en el menu inicial
    connect_to_server("localhost", "6000");
    connexion_controller->control(browser);

    // TODO(Franco):
    // esto simula un evento donde se pasa del browser al lobby
    // connexion_controller
    assert(connexion_controller != nullptr);
    connexion_controller->decontrol(browser);
    connexion_controller->control(lobby);

    // TODO(Franco): borrar cuando esté el quit = true en algun lado, lo pongo
    // para que el linter no moleste
    quit = false;
}

QtApp::~QtApp() {
    connexion_controller->decontrol(browser);
    connexion_controller->decontrol(lobby);
}

void QtApp::connect_to_server(const std::string& hostname,
                              const std::string& port) {
    connexion_controller = std::make_unique<ConnexionController>(
        Protocol(hostname.c_str(), port.c_str()));
}
