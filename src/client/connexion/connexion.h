#pragma once

#include "client/connexion/receiver.h"
#include "client/connexion/sender.h"

class Connexion final {
    Protocol protocol;
    Receiver receiver;
    Sender sender;

   public:
    explicit Connexion(Protocol&& protocol);

    MAKE_FIXED(Connexion)

    PlayerId unique_id;

    Api& get_api();

    struct Responder : Receiver::Listener {
       protected:
        void subscribe(Connexion&);
    };

    ~Connexion();

   private:
    [[nodiscard]] bool is_alive() const;
};
