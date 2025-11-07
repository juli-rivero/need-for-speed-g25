#pragma once

#include <unordered_map>

#include "../../common/emitter.h"
#include "../../common/macros.h"
#include "spdlog/spdlog.h"

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
    spdlog::logger* log;

    const std::unordered_map<int, UserSetup>& users_setup;

    friend struct GameListener;
    Emitter<GameListener> emitter;

   public:
    Game(const std::unordered_map<int, UserSetup>& users_setup,
         spdlog::logger*);

    MAKE_FIXED(Game)
};
