#pragma once

#include "common/dto/dto.h"
#include "common/dto/dto_lobby.h"
#include "common/dto/dto_session.h"
#include "common/macros.h"
#include "common/protocol.h"
#include "common/queue.h"
#include "common/thread.h"

class Sender final : public Thread {
    Queue<dto::Request> responses;
    ProtocolSender& sender;

    friend class ConnexionController;
    void run() override;
    void stop() override;

   public:
    explicit Sender(ProtocolSender&);

    MAKE_FIXED(Sender)

    void send(const dto_session::SearchRequest&);
    void send(const dto_session::JoinRequest&);
    void send(const dto_session::LeaveRequest&);
    void send(const dto_lobby::StartRequest&);
};
