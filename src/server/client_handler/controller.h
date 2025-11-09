#pragma once

#include <memory>

#include "common/dto/dto_search.h"
#include "common/dto/dto_session.h"
#include "controllers/game_controller.h"
#include "controllers/search_controller.h"
#include "controllers/session_controller.h"
#include "server/client_handler/receiver.h"

class Controller final : public ISearchEvents,
                         public ISessionEvents,
                         public IGameEvents {
    spdlog::logger* log;
    int id;
    Api& api;
    Receiver& receiver;

    template <typename T>
    using ptr = std::unique_ptr<T>;

    ptr<SearchController> search_controller;
    ptr<SessionController> session_controller;
    ptr<GameController> game_controller;

   public:
    explicit Controller(SessionsMonitor& monitor, int id, Api&, Receiver&,
                        spdlog::logger*);

    MAKE_FIXED(Controller)

   private:
    // BROWSER EVENTS //
    void on_join_session(Session&) override;

    // LOBBY EVENTS //
    void on_start_game(Game& game) override;

    // GAME EVENTS //
    void on_game_end() override;
};
