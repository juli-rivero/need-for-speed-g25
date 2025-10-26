#include "client/qt/qt_app.h"

QtApp::QtApp(ConnexionController& connexion_controller, bool& quit)
    : connexion_controller(connexion_controller) {
    // TODO(Franco):
    // hacer controlar el browser al crearlo
    connexion_controller.control(browser);

    // TODO(Franco):
    // esto simula un evento donde se pasa del browser al lobby
    // connexion_controller
    connexion_controller.decontrol(browser);
    connexion_controller.control(lobby);

    // TODO(Franco): borrar cuando esté el quit = true en algun lado, lo pongo
    // para que el linter no moleste
    quit = false;
}

QtApp::~QtApp() {
    connexion_controller.decontrol(browser);
    connexion_controller.decontrol(lobby);
}
