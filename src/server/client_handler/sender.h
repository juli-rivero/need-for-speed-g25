#pragma once

#include <spdlog/spdlog.h>

#include <string>
#include <vector>

#include "common/dto/dto.h"
#include "common/macros.h"
#include "common/protocol.h"
#include "common/queue.h"
#include "common/structs.h"
#include "common/thread.h"

struct Api {
    virtual void reply_search(const std::vector<SessionInfo>&) = 0;
    virtual void reply_joined(const SessionInfo& session,
                              const std::vector<CarStaticInfo>& carTypes) = 0;
    virtual void reply_left() = 0;
    virtual void reply_started() = 0;
    virtual void reply_error(const std::string&) = 0;
    virtual ~Api() = default;
};

class Sender final : public Thread, public Api {
    Queue<dto::Response> responses;
    ProtocolSender& sender;
    spdlog::logger* log;

    friend class ClientHandler;
    void run() override;
    void stop() override;

   public:
    explicit Sender(ProtocolSender&, spdlog::logger*);

    MAKE_FIXED(Sender)

    void reply_search(const std::vector<SessionInfo>&) override;
    void reply_joined(const SessionInfo& session,
                      const std::vector<CarStaticInfo>& carTypes) override;
    void reply_left() override;
    void reply_started() override;
    void reply_error(const std::string&) override;
};
