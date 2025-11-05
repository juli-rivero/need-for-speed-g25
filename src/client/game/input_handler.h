#pragma once

#include "client/connexion/connexion_controller.h"

class InputHandler {
    ConnexionController& connexion_controller;

   public:
    explicit InputHandler(ConnexionController& connexion_controller);

    MAKE_FIXED(InputHandler)
};
