#include "server/client_handler/client_handler.h"

#include <utility>

ClientHandler::ClientHandler(const int id, Socket&& socket,
                             SessionsMonitor& sessions_monitor)
    : protocol(std::move(socket)),
      sender(this->protocol),
      controller(sessions_monitor, id, this->sender),
      receiver(this->protocol, this->controller) {}

void ClientHandler::start() {
    receiver.start();
    sender.start();
}

bool ClientHandler::is_alive() const {
    return receiver.is_alive() or sender.is_alive();
}

void ClientHandler::kill() {
    if (sender.is_alive()) {
        sender.kill();
    }
    if (receiver.is_alive()) {
        receiver.kill();
    }
}

void ClientHandler::join() {
    sender.join();
    receiver.join();
}
