#pragma once

#include "client/game/game.h"
#include "common/macros.h"

class GameController final {
    Game& game;

   public:
    explicit GameController(Game& game);

    MAKE_FIXED(GameController)
};
