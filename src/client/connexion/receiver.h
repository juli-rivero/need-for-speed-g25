#pragma once

#include "common/dto/dto.h"
#include "common/dto/dto_lobby.h"
#include "common/dto/dto_session.h"
#include "common/macros.h"
#include "common/protocol.h"
#include "common/thread.h"

struct IResponseListener {
    virtual void recv(const dto::ErrorResponse&) = 0;
    virtual void recv(const dto_session::JoinResponse&) = 0;
    virtual void recv(const dto_session::LeaveResponse&) = 0;
    virtual void recv(const dto_session::SearchResponse&) = 0;
    virtual void recv(const dto_lobby::StartResponse&) = 0;
    virtual ~IResponseListener() = default;
};

class Receiver final : public Thread {
    ProtocolReceiver& receiver;
    IResponseListener& listener;

   public:
    Receiver(ProtocolReceiver& receiver, IResponseListener& listener);

    MAKE_FIXED(Receiver)

    void run() override;

    void stop() override;

   private:
    void delegate_response(const dto::ResponseType& request) const;
};
