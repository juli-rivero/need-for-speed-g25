#include "server/sessions_monitor.h"

#include <mutex>
#include <ranges>

#include "spdlog/sinks/stdout_color_sinks-inl.h"
#include "spdlog/spdlog.h"

SessionsMonitor::SessionsMonitor(YamlGameConfig& yaml_config)
    : log(spdlog::stdout_color_mt("SessionsMonitor")),
      yaml_config(yaml_config) {}

Session& SessionsMonitor::get_session(const std::string& id) {
    std::lock_guard lock(mtx);
    log->trace("get_session by session name {}", id);
    return sessions.at(id);
}

Session& SessionsMonitor::get_session(const int client_id) {
    std::lock_guard lock(mtx);
    log->trace("get_session by client id {}", client_id);
    return *searcher.at(client_id);
}

void SessionsMonitor::create_session(const SessionConfig& config,
                                     const int client_id) {
    std::lock_guard lock(mtx);
    sessions.emplace(std::piecewise_construct,
                     std::forward_as_tuple(config.name),
                     std::forward_as_tuple(config, client_id, yaml_config));
    searcher[client_id] = &sessions.at(config.name);
    log->trace("created session {} by {}", config.name, client_id);
}

void SessionsMonitor::leave_session(const int client_id) {
    std::lock_guard lock(mtx);
    log->trace("client {} left a session", client_id);
    searcher.at(client_id)->remove_client(client_id);
    searcher.erase(client_id);
}

std::vector<std::string> SessionsMonitor::get_sessions_ids() {
    std::lock_guard lock(mtx);
    std::vector<std::string> result;
    for (const std::string& id : sessions | std::views::keys) {
        result.push_back(id);
        log->trace("found session: {}", id);
    }
    log->trace("found {} sessions", result.size());
    return result;
}
