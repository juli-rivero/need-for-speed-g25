#pragma once

#include <memory>
#include <string>

#include "client/connexion/connexion_controller.h"
#include "client/qt/browser.h"
#include "client/qt/lobby.h"
#include "common/macros.h"

class QtApp final {
    std::unique_ptr<ConnexionController>& connexion_controller;
    Browser browser;
    Lobby lobby;

   public:
    QtApp(std::unique_ptr<ConnexionController>& connexion_controller,
          bool& quit);
    ~QtApp();

    MAKE_FIXED(QtApp)

   private:
    void connect_to_server(const std::string& hostname,
                           const std::string& port);
};
