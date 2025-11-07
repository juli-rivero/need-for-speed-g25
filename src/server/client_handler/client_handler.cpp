#include "client_handler.h"

#include <utility>

#include "spdlog/sinks/stdout_color_sinks-inl.h"
#include "spdlog/spdlog.h"

using spdlog::stdout_color_mt;
using std::to_string;

ClientHandler::ClientHandler(const int id, Socket&& socket,
                             SessionsMonitor& sessions_monitor)
    : log(stdout_color_mt("Client " + to_string(id))),
      protocol(std::move(socket)),
      sender(this->protocol, log.get()),
      controller(sessions_monitor, id, this->sender, log.get()),
      receiver(this->protocol, this->controller, log.get()) {
    log->debug("Created");
    receiver.start();
    sender.start();
}

bool ClientHandler::is_alive() const {
    return receiver.is_alive() and sender.is_alive();
}

ClientHandler::~ClientHandler() {
    if (sender.is_alive()) {
        sender.stop();
    }
    if (receiver.is_alive()) {
        receiver.stop();
    }
    sender.join();
    receiver.join();
    log->debug("Destroyed");
}
