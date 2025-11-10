#pragma once

#include <unordered_map>

#include "../../common/emitter.h"
#include "../../common/macros.h"
#include "spdlog/spdlog.h"

struct UserSetup {
    bool ready;
};

class Game;

class Game final {
    spdlog::logger* log;

    const std::unordered_map<int, UserSetup>& users_setup;

   public:
    Game(const std::unordered_map<int, UserSetup>& users_setup,
         spdlog::logger*);

    MAKE_FIXED(Game)

    struct Listener : common::Listener<Game::Listener> {
        Game& game;

        explicit Listener(Game& game);

        virtual void on_end_game() = 0;

        virtual void on_snapshot() = 0;

        ~Listener() override = default;
    };

   private:
    common::Emitter<Game::Listener> emitter;
};
