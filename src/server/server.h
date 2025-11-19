#pragma once

#include <string>

#include "../common/macros.h"
#include "config/YamlGameConfig.h"
#include "server/acceptor.h"

class Server {
    SessionsMonitor sessions_monitor;
    ClientsManager clients_manager;
    Acceptor acceptor;

   public:
    explicit Server(const std::string& port, YamlGameConfig& config);

    MAKE_FIXED(Server)

    ~Server();
};
