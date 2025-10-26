#pragma once

#include <mutex>

#include "client/connexion/browser_controller.h"
#include "client/connexion/game_controller.h"
#include "client/connexion/lobby_controller.h"
#include "client/connexion/receiver.h"

class ResponseController : public ResponseListener {
    BrowserController* browser_controller;
    LobbyController* lobby_controller;
    GameController* game_controller;

    std::mutex mtx;

   public:
    ResponseController();

    MAKE_FIXED(ResponseController)

    void control(Browser& browser);
    void decontrol(Browser& browser);

    void control(Lobby& lobby);
    void decontrol(Lobby& lobby);

    void control(Game& game);
    void decontrol(Game& game);

    void decontrol_all();

    ~ResponseController() override;

   private:
    // BROWSER CONTROLLER //
    void on(const dto_session::LeaveResponse&) override;
    void on(const dto_session::JoinResponse&) override;
    void on(const dto_session::SearchResponse&) override;

    // LOBBY CONTROLLER //
    void on(const dto_lobby::StartResponse&) override;

    void on(const dto::ErrorResponse&) override;
};
