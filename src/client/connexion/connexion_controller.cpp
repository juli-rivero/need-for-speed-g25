#include "client/connexion/connexion_controller.h"

#include <utility>

#include "spdlog/spdlog.h"

ConnexionController::ConnexionController(Protocol&& protocol)
    : ResponseController(),
      RequestController(sender),
      protocol(std::move(protocol)),
      receiver(this->protocol, *this),
      sender(this->protocol) {
    receiver.start();
    sender.start();
}

ConnexionController::~ConnexionController() {
    if (sender.is_alive()) sender.stop();
    if (receiver.is_alive()) receiver.stop();
    sender.join();
    receiver.join();
    spdlog::trace("Connexion controller destroyed");
}

bool ConnexionController::is_alive() const {
    return receiver.is_alive() or sender.is_alive();
}
