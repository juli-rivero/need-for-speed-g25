#pragma once

#include <common/emitter.h>

#include <string>
#include <vector>

#include "common/dto/dto.h"
#include "common/macros.h"
#include "common/protocol.h"
#include "common/thread.h"

class Receiver;

class Receiver final : public Thread {
    ProtocolReceiver& receiver;

    friend class Connexion;
    void run() override;
    void stop() override;

   public:
    explicit Receiver(ProtocolReceiver& receiver);

    MAKE_FIXED(Receiver)

    struct Listener : common::Listener<Receiver::Listener> {
        explicit Listener(Receiver& receiver);
        virtual void on_error_response(const std::string&) {}
        virtual void on_search_response(const std::vector<SessionInfo>&) {}
        virtual void on_join_response(const SessionInfo&,
                                      const std::vector<CarStaticInfo>&) {}
        virtual void on_leave_response() {}
        virtual void on_start_response() {}
        ~Listener() override = default;
    };

   private:
    common::Emitter<Receiver::Listener> emitter;
    void delegate_response(const dto::ResponseType& response);
};
