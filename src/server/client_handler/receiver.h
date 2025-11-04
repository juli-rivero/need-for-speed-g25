#pragma once

#include "common/dto/dto.h"
#include "common/macros.h"
#include "common/protocol.h"
#include "common/thread.h"
#include "spdlog/spdlog.h"

struct RequestListener {
    virtual void on(const dto_search::JoinRequest&) = 0;
    virtual void on(const dto_search::SearchRequest&) = 0;
    virtual void on(const dto_session::LeaveRequest&) = 0;
    virtual void on(const dto_session::StartRequest&) = 0;

    virtual ~RequestListener() = default;
};

class Receiver final : public Thread {
    ProtocolReceiver& receiver;
    RequestListener& listener;
    spdlog::logger* log;

   public:
    Receiver(ProtocolReceiver&, RequestListener&, spdlog::logger*);

    MAKE_FIXED(Receiver)

    void run() override;

    void stop() override;

   private:
    void switch_and_dispatch_request(const dto::RequestType& request) const;
};
