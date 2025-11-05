#pragma once

#include "common/dto/dto.h"
#include "common/dto/dto_search.h"
#include "common/dto/dto_session.h"
#include "common/macros.h"
#include "common/protocol.h"
#include "common/queue.h"
#include "common/thread.h"

class Sender final : public Thread {
    Queue<dto::Request> responses;
    ProtocolSender& sender;

    friend class Connexion;
    void run() override;
    void stop() override;

   public:
    explicit Sender(ProtocolSender&);

    MAKE_FIXED(Sender)

    void send(const dto_search::SearchRequest&);
    void send(const dto_search::JoinRequest&);
    void send(const dto_search::CreateRequest&);
    void send(const dto_session::LeaveRequest&);
    void send(const dto_session::StartRequest&);
};
