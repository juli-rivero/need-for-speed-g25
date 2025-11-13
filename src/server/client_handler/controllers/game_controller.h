#pragma once

#include "server/client_handler/receiver.h"
#include "server/client_handler/sender.h"
#include "server/session/logic/GameSessionFacade.h"

struct IGameEvents {
    virtual void on_game_end() = 0;
    virtual ~IGameEvents() = default;
};

class GameController final : Receiver::Listener {
    spdlog::logger* log;
    const PlayerId client_id;
    Api& api;
    IGameEvents& dispatcher;

   public:
    GameController(GameSessionFacade&, PlayerId client_id, Api&, Receiver&,
                   IGameEvents& handler, spdlog::logger*);

    MAKE_FIXED(GameController)
};
