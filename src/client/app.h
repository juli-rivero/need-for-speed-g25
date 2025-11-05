#pragma once

#include <string>

#include "common/macros.h"
#include "connexion/connexion_controller.h"

class App {
    ConnexionController connexion_controller;

   public:
    explicit App(const std::string& host, const std::string& port);
    ~App();
    MAKE_FIXED(App)

    void run();
};
