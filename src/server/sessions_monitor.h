#pragma once

#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

#include "common/macros.h"
#include "config/YamlGameConfig.h"
#include "session/session.h"
#include "spdlog/spdlog.h"

class SessionsMonitor {
    std::shared_ptr<spdlog::logger> log;
    std::unordered_map<std::string, Session> sessions;

    std::mutex mtx;

    YamlGameConfig& yaml_config;

   public:
    explicit SessionsMonitor(YamlGameConfig&);

    MAKE_FIXED(SessionsMonitor)

    Session& get_session(const std::string& id);

    void create_session(const SessionConfig&, PlayerId client_id);

    std::vector<std::string> get_sessions_ids();

   private:
    void reap_sessions();
};
