#pragma once

#include "common/dto/dto_session.h"
#include "server/client_handler/sender.h"
#include "server/sessions_monitor.h"

struct IBrowserEvents {
    virtual void on_join_session(Session&) = 0;
    virtual ~IBrowserEvents() = default;
};

class BrowserController final {
    SessionsMonitor& sessions_monitor;
    const int client_id;
    Sender& sender;
    IBrowserEvents& dispatcher;

   public:
    BrowserController(SessionsMonitor& sessions_monitor, int client_id,
                      Sender& sender, IBrowserEvents& handler);

    MAKE_FIXED(BrowserController)

    void on(const dto_session::LeaveRequest&) const;
    void on(const dto_session::JoinRequest&) const;
    void on(const dto_session::SearchRequest&) const;
};
