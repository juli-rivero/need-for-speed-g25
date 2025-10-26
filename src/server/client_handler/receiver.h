#pragma once

#include "common/dto/dto.h"
#include "common/macros.h"
#include "common/protocol.h"
#include "common/thread.h"

struct RequestListener {
    virtual void on(const dto_session::LeaveRequest&) = 0;
    virtual void on(const dto_session::JoinRequest&) = 0;
    virtual void on(const dto_session::SearchRequest&) = 0;
    virtual void on(const dto_lobby::StartRequest&) = 0;

    virtual ~RequestListener() = default;
};

class Receiver final : public Thread {
    ProtocolReceiver& receiver;
    RequestListener& listener;

   public:
    Receiver(ProtocolReceiver& receiver, RequestListener& listener);

    MAKE_FIXED(Receiver)

    void run() override;

    void kill();

   private:
    void switch_and_dispatch_request(const dto::RequestType& request) const;
};
