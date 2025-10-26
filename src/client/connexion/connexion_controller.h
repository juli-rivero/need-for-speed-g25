#pragma once

#include <utility>

#include "client/connexion/receiver.h"
#include "client/connexion/response_controller.h"
#include "client/connexion/sender.h"

class ConnexionController final : public ResponseController {
    Protocol protocol;
    Receiver receiver;
    Sender sender;

   public:
    explicit ConnexionController(Protocol&& protocol);

    MAKE_FIXED(ConnexionController)

    template <typename... Args>
    void send(Args&&... args) {
        sender.send(std::forward<Args>(args)...);
    }

   private:
    void start();
    [[nodiscard]] bool is_alive() const;
    void kill();
    void join();
};
