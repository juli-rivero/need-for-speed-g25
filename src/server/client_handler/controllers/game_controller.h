#pragma once

#include "server/client_handler/receiver.h"
#include "server/client_handler/sender.h"
#include "server/session/logic/GameSessionFacade.h"

struct IGameEvents {
    virtual void on_game_end() = 0;
    virtual ~IGameEvents() = default;
};

class GameController final : Receiver::Listener, GameSessionFacade::Listener {
    spdlog::logger* log;
    const PlayerId client_id;
    Api& api;
    IGameEvents& dispatcher;
    GameSessionFacade& game;

   public:
    GameController(GameSessionFacade&, PlayerId client_id, Api&, Receiver&,
                   IGameEvents& handler, spdlog::logger*);
    ~GameController() override;

    MAKE_FIXED(GameController)

    void on_turn_left() override;
    void on_stop_turning_left() override;
    void on_turn_right() override;
    void on_stop_turning_right() override;
    void on_accelerate() override;
    void on_stop_accelerating() override;
    void on_reverse() override;
    void on_stop_reversing() override;
    void on_nitro() override;

    void on_snapshot(const WorldSnapshot&) override;
    void on_collision_event(const CollisionEvent&) override;
};
