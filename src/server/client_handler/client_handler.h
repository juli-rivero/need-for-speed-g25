#pragma once

#include "server/client_handler/controller.h"
#include "server/client_handler/receiver.h"
#include "server/client_handler/sender.h"
#include "server/sessions_monitor.h"

class ClientHandler final {
    enum class Step {
        SearchLobby,
        InLobby,
        InGame,
    };

    Protocol protocol;
    Sender sender;
    Controller controller;
    Receiver receiver;

   public:
    ClientHandler(int id, Socket&& socket, SessionsMonitor& sessions_monitor);

    MAKE_FIXED(ClientHandler)

    void start();

    [[nodiscard]] bool is_alive() const;

    void kill();

    void join();
};
