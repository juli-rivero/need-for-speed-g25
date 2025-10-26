#pragma once

#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

#include "common/macros.h"
#include "server/session/session.h"

class SessionsMonitor {
    std::unordered_map<std::string, Session> sessions;
    std::unordered_map<int, Session*> searcher;

    std::mutex mtx;

   public:
    SessionsMonitor() = default;

    MAKE_FIXED(SessionsMonitor)

    Session& get_session(const std::string& id);

    Session& get_session(int client_id);

    void create_session(const std::string& id, int client_id);

    void leave_session(int client_id);

    std::vector<std::string> get_sessions_ids();
};
