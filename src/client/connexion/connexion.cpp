#include "client/connexion/connexion.h"

#include <utility>

#include "spdlog/spdlog.h"

Connexion::Connexion(Protocol&& protocol)
    : protocol(std::move(protocol)),
      receiver(this->protocol, *this),
      sender(this->protocol) {
    receiver.start();
    sender.start();
}

Connexion::~Connexion() {
    if (sender.is_alive()) sender.stop();
    if (receiver.is_alive()) receiver.stop();
    sender.join();
    receiver.join();
    spdlog::trace("Connexion controller destroyed");
}

bool Connexion::is_alive() const {
    return receiver.is_alive() or sender.is_alive();
}
