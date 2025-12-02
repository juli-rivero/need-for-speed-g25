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

void SessionsMonitor::create_session(const SessionConfig& config,
                                     const PlayerId client_id) {
    reap_sessions();

    std::lock_guard lock(mtx);
    if (sessions.contains(config.name))
        throw std::runtime_error("Session already exists");
    sessions.emplace(std::piecewise_construct,
                     std::forward_as_tuple(config.name),
                     std::forward_as_tuple(config, client_id, yaml_config));
    log->trace("created session {} by {}", config.name, client_id);
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

int SessionsMonitor::get_max_players_per_session() {
    return yaml_config.getMaxPlayers();
}
int SessionsMonitor::get_max_races_per_session() {
    return yaml_config.getMaxRacesPerSession();
}
std::vector<CityName> SessionsMonitor::get_available_cities() {
    return yaml_config.getAvailableCities();
}

void SessionsMonitor::reap_sessions() {
    for (auto it = sessions.begin(); it != sessions.end();) {
        if (it->second.empty() || it->second.finished())
            it = sessions.erase(it);
        else
            ++it;
    }
}
