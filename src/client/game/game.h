#pragma once

#include "common/macros.h"

class Game final {
   public:
    Game() = default;
    
    bool start();
    
    MAKE_FIXED(Game)
};
