#pragma once

#include "client/connexion/connexion_controller.h"
#include "client/qt/browser.h"
#include "client/qt/lobby.h"
#include "common/macros.h"

class QtApp final {
    ConnexionController& connexion_controller;
    Browser browser;
    Lobby lobby;

   public:
    QtApp(ConnexionController& connexion_controller, bool& quit);
    ~QtApp();

    MAKE_FIXED(QtApp)
};
