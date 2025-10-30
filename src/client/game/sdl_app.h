#pragma once

#include "client/connexion/connexion_controller.h"
#include "client/game/game.h"
#include "client/game/input_handler.h"

class SdlApp {
    ConnexionController& connexion_controller;
    InputHandler input_handler;

   public:
    explicit SdlApp(ConnexionController& connexion_controller, bool& quit);
    ~SdlApp();

    MAKE_FIXED(SdlApp)
};
