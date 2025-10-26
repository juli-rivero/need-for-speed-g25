#pragma once

#include "common/macros.h"

class App {
   public:
    App() = default;
    MAKE_FIXED(App)

    void run();
};
