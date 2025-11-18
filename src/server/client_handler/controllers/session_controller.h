#pragma once

#include <string>
#include <vector>

#include "server/client_handler/receiver.h"
#include "server/client_handler/sender.h"
#include "server/session/session.h"

struct ISessionEvents {
    virtual void on_start_game(GameSessionFacade& game) = 0;
    virtual void on_leave_session() = 0;
    virtual ~ISessionEvents() = default;
};

class SessionController final : Session::Listener, Receiver::Listener {
    spdlog::logger* log;
    const PlayerId client_id;
    Api& api;
    ISessionEvents& dispatcher;
    Session& session;

   public:
    SessionController(Session&, PlayerId client_id, Api&, Receiver&,
                      ISessionEvents& handler, spdlog::logger*);

    ~SessionController() override;

    MAKE_FIXED(SessionController)

    void on_start_game(GameSessionFacade& game) override;
    void on_start_request(bool ready) override;
    void on_choose_car(const std::string&) override;
    void on_leave_request() override;

    void on_session_updated(const SessionConfig&,
                            const std::vector<PlayerInfo>& players) override;
};
