#pragma once

#include <string>

#include "server/client_handler/receiver.h"
#include "server/client_handler/sender.h"
#include "server/sessions_monitor.h"

struct ISearchEvents {
    virtual void on_join_session(Session&) = 0;
    virtual ~ISearchEvents() = default;
};

class SearchController final : Receiver::Listener {
    spdlog::logger* log;
    SessionsMonitor& sessions_monitor;
    const int client_id;
    Api& api;
    ISearchEvents& dispatcher;

   public:
    SearchController(SessionsMonitor&, int client_id, Api&, Receiver&,
                     ISearchEvents& handler, spdlog::logger*);

    MAKE_FIXED(SearchController)

    void on_join_request(const std::string&) override;
    void on_search_request() override;
    void on_create_request(const SessionConfig&) override;
    void on_leave_request() override;
};
