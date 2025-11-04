#pragma once

#include "common/dto/dto.h"
#include "common/dto/dto_lobby.h"
#include "common/dto/dto_session.h"
#include "common/macros.h"
#include "common/protocol.h"
#include "common/thread.h"

struct ResponseListener {
    virtual void recv(const dto::ErrorResponse&) = 0;
    virtual void recv(const dto_session::JoinResponse&) = 0;
    virtual void recv(const dto_session::LeaveResponse&) = 0;
    virtual void recv(const dto_session::SearchResponse&) = 0;
    virtual void recv(const dto_lobby::StartResponse&) = 0;
    virtual ~ResponseListener() = default;
};

class Receiver final : public Thread {
    ProtocolReceiver& receiver;
    ResponseListener& listener;

    friend class Connexion;
    void run() override;
    void stop() override;

   public:
    Receiver(ProtocolReceiver& receiver, ResponseListener& listener);

    MAKE_FIXED(Receiver)

   private:
    void delegate_response(const dto::ResponseType& request) const;
};
