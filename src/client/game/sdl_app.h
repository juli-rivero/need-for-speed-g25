#pragma once

#include "client/connexion/connexion.h"
#include "client/game/input_handler.h"

class SdlApp {
    Connexion& connexion;
    InputHandler input_handler;

   public:
    explicit SdlApp(Connexion& connexion, bool& quit);
    ~SdlApp();

    MAKE_FIXED(SdlApp)
};
