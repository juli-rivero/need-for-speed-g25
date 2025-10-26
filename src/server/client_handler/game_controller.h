#pragma once

#include "server/client_handler/sender.h"
#include "server/session/game.h"

struct IGameEvents {
    virtual void on_game_end() = 0;
    virtual ~IGameEvents() = default;
};

class GameController final : GameListener {
    const int client_id;
    Sender& sender;
    IGameEvents& dispatcher;

   public:
    GameController(Game& game, int client_id, Sender& sender,
                   IGameEvents& handler);

    MAKE_FIXED(GameController)

    void on_end_game() override;
    void on_snapshot() override;
};
