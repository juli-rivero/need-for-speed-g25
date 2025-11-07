#pragma once

#include <memory>

#include "controller.h"
#include "receiver.h"
#include "sender.h"
#include "../sessions_monitor.h"
#include "../../common/protocol.h"

class ClientHandler final {
    enum class Step {
        SearchLobby,
        InLobby,
        InGame,
    };
    std::shared_ptr<spdlog::logger> log;
    Protocol protocol;
    Sender sender;
    Controller controller;
    Receiver receiver;

   public:
    ClientHandler(int id, Socket&& socket, SessionsMonitor& sessions_monitor);

    MAKE_FIXED(ClientHandler)

    [[nodiscard]] bool is_alive() const;

    void kill();

    ~ClientHandler();
};
