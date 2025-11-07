#include "server.h"

#include <string>

Server::Server(const std::string& port) : acceptor(port, clients_manager) {
    acceptor.start();
}

Server::~Server() {
    acceptor.stop();
    acceptor.join();

    clients_manager.kill_all();
}
