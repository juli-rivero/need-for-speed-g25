#include "client/connexion/connexion_controller.h"

#include <utility>

ConnexionController::ConnexionController(Protocol&& protocol)
    : protocol(std::move(protocol)),
      receiver(this->protocol, *this),
      sender(this->protocol) {}

void ConnexionController::start() {
    receiver.start();
    sender.start();
}

bool ConnexionController::is_alive() const {
    return receiver.is_alive() or sender.is_alive();
}

void ConnexionController::kill() {
    if (sender.is_alive()) sender.kill();
    if (receiver.is_alive()) receiver.kill();
}

void ConnexionController::join() {
    sender.join();
    receiver.join();
}
