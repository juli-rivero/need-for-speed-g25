#pragma once

#include <string>

#include "common/macros.h"
#include "connexion/connexion.h"

class App {
    Connexion connexion;

   public:
    explicit App(const std::string& host, const std::string& port);
    ~App();
    MAKE_FIXED(App)

    void run();
};
