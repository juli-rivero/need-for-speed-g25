#pragma once

#include "common/macros.h"

class Game;

class GameController final {
    Game& game;

   public:
    explicit GameController(Game& game);

    MAKE_FIXED(GameController)
};
