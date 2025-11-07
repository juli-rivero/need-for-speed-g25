#pragma once

#include "sender.h"
#include "../session/game.h"

struct IGameEvents {
    virtual void on_game_end() = 0;
    virtual ~IGameEvents() = default;
};

class GameController final : GameListener {
    spdlog::logger* log;
    const int client_id;
    Sender& sender;
    IGameEvents& dispatcher;

   public:
    GameController(Game&, int client_id, Sender&, IGameEvents& handler,
                   spdlog::logger*);

    MAKE_FIXED(GameController)

    void on_end_game() override;
    void on_snapshot() override;
};
