#pragma once

#include "common/dto/dto_session.h"
#include "server/client_handler/sender.h"
#include "server/sessions_monitor.h"

struct ISearchEvents {
    virtual void on_join_session(Session&) = 0;
    virtual ~ISearchEvents() = default;
};

class SearchController final {
    spdlog::logger* log;
    SessionsMonitor& sessions_monitor;
    const int client_id;
    Sender& sender;
    ISearchEvents& dispatcher;

   public:
    SearchController(SessionsMonitor&, int client_id, Sender&,
                     ISearchEvents& handler, spdlog::logger*);

    MAKE_FIXED(SearchController)

    void on(const dto_search::JoinRequest&) const;
    void on(const dto_search::SearchRequest&) const;
};
