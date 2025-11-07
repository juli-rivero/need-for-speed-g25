#pragma once

#include <list>

#include "../common/socket.h"
#include "client_handler/client_handler.h"
#include "sessions_monitor.h"

class ClientsManager {
    std::list<ClientHandler> client_handlers;
    SessionsMonitor sessions_monitor;
    std::mutex mutex;
    int next_id = 0;

   public:
    ClientsManager() = default;
    MAKE_FIXED(ClientsManager)

    void manage_new_handler(Socket&& socket);

    void reap_dead();

    void kill_all();

    ~ClientsManager();
};
