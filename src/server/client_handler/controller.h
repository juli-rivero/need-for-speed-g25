#pragma once

#include "common/dto/dto_search.h"
#include "common/dto/dto_session.h"
#include "controllers/game_controller.h"
#include "controllers/search_controller.h"
#include "controllers/session_controller.h"
#include "server/client_handler/receiver.h"

class Controller final : public RequestListener,
                         ISearchEvents,
                         ISessionEvents,
                         IGameEvents {
    spdlog::logger* log;
    int id;
    Sender& sender;

    SearchController* search_controller;
    SessionController* session_controller;
    GameController* game_controller;

    std::mutex mtx;

   public:
    explicit Controller(SessionsMonitor& monitor, int id, Sender& sender,
                        spdlog::logger*);

    MAKE_FIXED(Controller)

    // BROWSER CONTROLLER //
    void on(const dto_search::JoinRequest&) override;
    void on(const dto_search::SearchRequest&) override;

    // LOBBY CONTROLLER //
    void on(const dto_session::LeaveRequest&) override;
    void on(const dto_session::StartRequest&) override;

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
