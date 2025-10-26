#pragma once

#include "common/macros.h"
#include "connexion/connexion_controller.h"

class App {
    ConnexionController connexion_controller;

   public:
    explicit App(const char* host = "localhost", const char* port = "6457");
    MAKE_FIXED(App)

    void run();
};
