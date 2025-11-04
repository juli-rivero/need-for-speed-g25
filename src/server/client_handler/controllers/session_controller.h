#pragma once

#include "common/dto/dto_session.h"
#include "server/client_handler/sender.h"
#include "server/session/session.h"

struct ISessionEvents {
    virtual void on_start_game(Game& game) = 0;
    virtual ~ISessionEvents() = default;
};

class SessionController final : SessionListener {
    spdlog::logger* log;
    const int client_id;
    Sender& sender;
    ISessionEvents& dispatcher;

   public:
    SessionController(Session&, int client_id, Sender&, ISessionEvents& handler,
                      spdlog::logger*);

    MAKE_FIXED(SessionController)

    void on(const dto_session::LeaveRequest&) const;
    void on(const dto_session::StartRequest&);

    void on_start_game(Game& game) override;
};
