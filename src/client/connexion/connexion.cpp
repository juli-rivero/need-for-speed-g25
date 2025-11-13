#include "client/connexion/connexion.h"

#include <utility>

#include "spdlog/spdlog.h"

Connexion::Connexion(Protocol&& protocol)
    : protocol(std::move(protocol)),
      receiver(this->protocol),
      sender(this->protocol),
      unique_id(this->protocol.get<PlayerId>()) {
    receiver.start();
    sender.start();
}

Api& Connexion::get_api() { return sender; }

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
