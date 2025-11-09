#pragma once

#include <memory>

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
    std::shared_ptr<spdlog::logger> log;
    Protocol protocol;
    Sender sender;
    Receiver receiver;
    Controller controller;

   public:
    ClientHandler(int id, Socket&& socket, SessionsMonitor& sessions_monitor);

    MAKE_FIXED(ClientHandler)

    [[nodiscard]] bool is_alive() const;

    ~ClientHandler();
};
