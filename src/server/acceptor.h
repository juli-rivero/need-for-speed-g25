#pragma once

#include <string>

#include "../common/socket.h"
#include "server/clients_manager.h"

class Acceptor final : public Thread {
    Socket acceptor;
    ClientsManager& clients_manager;

   public:
    explicit Acceptor(const std::string& port, ClientsManager& clients_manager);

    MAKE_FIXED(Acceptor)

    void run() override;

    void stop() override;
};
