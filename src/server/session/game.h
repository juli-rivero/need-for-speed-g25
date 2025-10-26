#pragma once

#include <unordered_map>

#include "common/emitter.h"
#include "common/macros.h"
#include "server/session/game_loop.h"

struct UserSetup {
    bool ready;
};

class Game;

struct GameListener : Listener<GameListener> {
    Game& game;

    explicit GameListener(Game& game);

    virtual void on_end_game() = 0;

    virtual void on_snapshot() = 0;

    ~GameListener() override = default;
};

class Game final {
    const std::unordered_map<int, UserSetup>& users_setup;
    GameLoop game_loop;

    friend struct GameListener;
    Emitter<GameListener> emitter;

   public:
    explicit Game(const std::unordered_map<int, UserSetup>& users_setup);

    MAKE_FIXED(Game)
};
