#pragma once

#include "client/connexion/connexion.h"

class InputHandler {
    Connexion& connexion;

   public:
    explicit InputHandler(Connexion& connexion);

    MAKE_FIXED(InputHandler)
};
