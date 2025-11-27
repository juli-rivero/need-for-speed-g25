#include "server/server.h"

#include <string>

Server::Server(const std::string& port, YamlGameConfig& config)
    : sessions_monitor(config),
      clients_manager(sessions_monitor, config),
      acceptor(port, clients_manager) {
    acceptor.start();
}

Server::~Server() {
    acceptor.stop();
    acceptor.join();

    clients_manager.kill_all();
}
