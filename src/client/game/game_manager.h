#pragma once

#include <memory>

#include "client/game/game.h"

class GameManager {
    std::unique_ptr<Game> impl;

   public:
    GameManager();
    ~GameManager();
};
