#pragma once

#include "common/dto/dto_lobby.h"
#include "common/dto/dto_session.h"
#include "server/client_handler/browser_controller.h"
#include "server/client_handler/game_controller.h"
#include "server/client_handler/lobby_controller.h"
#include "server/client_handler/receiver.h"

class Controller final : public RequestListener,
                         IBrowserEvents,
                         ILobbyEvents,
                         IGameEvents {
    spdlog::logger* log;
    int id;
    Sender& sender;

    BrowserController* browser_controller;
    LobbyController* lobby_controller;
    GameController* game_controller;

    std::mutex mtx;

   public:
    explicit Controller(SessionsMonitor& monitor, int id, Sender& sender,
                        spdlog::logger*);

    MAKE_FIXED(Controller)

    // BROWSER CONTROLLER //
    void on(const dto_session::LeaveRequest&) override;
    void on(const dto_session::JoinRequest&) override;
    void on(const dto_session::SearchRequest&) override;

    // LOBBY CONTROLLER //
    void on(const dto_lobby::StartRequest&) override;

    ~Controller() override;

   private:
    void decontrol_all();

    // BROWSER EVENTS //
    void on_join_session(Session&) override;

    // LOBBY EVENTS //
    void on_start_game(Game& game) override;

    // GAME EVENTS //
    void on_game_end() override;
};
