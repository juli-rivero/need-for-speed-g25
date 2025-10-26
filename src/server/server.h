#pragma once

#include <string>

#include "common/macros.h"
#include "server/acceptor.h"

class Server {
    ClientsManager clients_manager;
    Acceptor acceptor;

   public:
    explicit Server(const std::string& port);

    MAKE_FIXED(Server)

    ~Server();
};
