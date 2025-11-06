#pragma once

#include <string>

#include "server/client_handler/receiver.h"
#include "server/client_handler/sender.h"
#include "server/session/session.h"

struct ISessionEvents {
    virtual void on_start_game(Game& game) = 0;
    virtual ~ISessionEvents() = default;
};

class SessionController final : Session::Listener, Receiver::Listener {
    spdlog::logger* log;
    const int client_id;
    Api& api;
    ISessionEvents& dispatcher;

   public:
    SessionController(Session&, int client_id, Api&, Receiver&,
                      ISessionEvents& handler, spdlog::logger*);

    MAKE_FIXED(SessionController)

    void on_start_game(Game& game) override;

    void on_start_request(bool ready) override;
};
