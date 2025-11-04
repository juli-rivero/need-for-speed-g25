#pragma once

#include "client/connexion/receiver.h"
#include "client/connexion/response_controller.h"
#include "client/connexion/sender.h"

class Connexion final : public ResponseController {
    Protocol protocol;
    Receiver receiver;

    friend class Requester;
    Sender sender;

   public:
    explicit Connexion(Protocol&& protocol);

    MAKE_FIXED(Connexion)

    ~Connexion() override;

    using ResponseController::control;
    using ResponseController::decontrol;

   private:
    [[nodiscard]] bool is_alive() const;
};

class Requester {
   protected:
    Sender& sender;

   public:
    explicit Requester(Connexion& connexion) : sender(connexion.sender) {}
};

class MockRequester {
   protected:
    ResponseListener& response_listener;

   public:
    explicit MockRequester(Connexion& connexion)
        : response_listener(connexion) {}
};
