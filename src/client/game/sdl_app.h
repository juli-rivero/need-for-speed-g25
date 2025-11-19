#pragma once

#include "client/connexion/connexion.h"
#include "client/constants.h"
#include "client/game/input_handler.h"

class SdlApp {
    Connexion& connexion;
    InputHandler input_handler;

   public:
    explicit SdlApp(Connexion& connexion, bool& quit, GameSetUp& setup);
    ~SdlApp();

    MAKE_FIXED(SdlApp)
};
