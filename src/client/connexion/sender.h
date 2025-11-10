#pragma once

#include <string>

#include "common/dto/dto.h"
#include "common/macros.h"
#include "common/protocol.h"
#include "common/queue.h"
#include "common/structs.h"
#include "common/thread.h"

struct Api {
    virtual void request_search_all_sessions() = 0;
    virtual void request_join_session(const std::string&) = 0;
    virtual void request_create_and_join_session(const SessionConfig&) = 0;
    virtual void request_leave_current_session() = 0;
    virtual void request_start_session() = 0;
    virtual ~Api() = default;
};

class Sender final : public Thread, public Api {
    Queue<dto::Request> responses;
    ProtocolSender& sender;

    friend class Connexion;
    void run() override;
    void stop() override;

   public:
    explicit Sender(ProtocolSender&);

    MAKE_FIXED(Sender)

    void request_search_all_sessions() override;
    void request_join_session(const std::string&) override;
    void request_create_and_join_session(const SessionConfig&) override;
    void request_leave_current_session() override;
    void request_start_session() override;
};
