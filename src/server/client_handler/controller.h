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
                         public IGameEvents,
                         public Thread {
    spdlog::logger* log;
    int id;
    Api& api;
    Receiver& receiver;

    Queue<std::function<void()>> events;

    template <typename T>
    using ptr = std::unique_ptr<T>;

    ptr<SearchController> search_controller;
    ptr<SessionController> session_controller;
    ptr<GameController> game_controller;

   public:
    explicit Controller(SessionsMonitor& monitor, int id, Api&, Receiver&,
                        spdlog::logger*);

    MAKE_FIXED(Controller)

    void stop() override;

   private:
    void run() override;

    // BROWSER EVENTS //
    void on_join_session(Session&) override;

    // LOBBY EVENTS //
    void on_start_game(GameSessionFacade& game) override;

    // GAME EVENTS //
    void on_game_end() override;
};
