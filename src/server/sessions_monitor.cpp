#include "server/sessions_monitor.h"

#include <mutex>
#include <ranges>

Session& SessionsMonitor::get_session(const std::string& id) {
    std::lock_guard lock(mtx);
    return sessions.at(id);
}

Session& SessionsMonitor::get_session(const int client_id) {
    std::lock_guard lock(mtx);
    return *searcher.at(client_id);
}

void SessionsMonitor::create_session(const std::string& id,
                                     const int client_id) {
    std::lock_guard lock(mtx);
    sessions.emplace(id, client_id);
    searcher[client_id] = &sessions.at(id);
}

void SessionsMonitor::leave_session(const int client_id) {
    std::lock_guard lock(mtx);
    searcher.at(client_id)->remove_client(client_id);
    searcher.erase(client_id);
}

std::vector<std::string> SessionsMonitor::get_sessions_ids() {
    std::lock_guard lock(mtx);
    std::vector<std::string> result;
    for (const std::string& id : sessions | std::views::keys) {
        result.push_back(id);
    }
    return result;
}
