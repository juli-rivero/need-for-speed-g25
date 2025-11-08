#pragma once

#include "../../common/dto/dto_session.h"
#include "../sessions_monitor.h"
#include "server/client_handler/sender.h"

struct IBrowserEvents {
    virtual void on_join_session(Session&) = 0;
    virtual ~IBrowserEvents() = default;
};

class BrowserController final {
    spdlog::logger* log;
    SessionsMonitor& sessions_monitor;
    const int client_id;
    Sender& sender;
    IBrowserEvents& dispatcher;

   public:
    BrowserController(SessionsMonitor&, int client_id, Sender&,
                      IBrowserEvents& handler, spdlog::logger*);

    MAKE_FIXED(BrowserController)

    void on(const dto_session::LeaveRequest&) const;
    void on(const dto_session::JoinRequest&) const;
    void on(const dto_session::SearchRequest&) const;
};
