#pragma once

#include "client/connexion/connexion.h"
#include "client/constants.h"

class SdlApp {
   public:
    explicit SdlApp(Connexion& connexion, bool& quit, GameSetUp& setup);
    ~SdlApp();

    MAKE_FIXED(SdlApp)
};
