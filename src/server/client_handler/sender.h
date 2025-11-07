#pragma once

#include <spdlog/spdlog.h>

#include "../../common/dto/dto.h"
#include "../../common/dto/dto_lobby.h"
#include "../../common/dto/dto_session.h"
#include "../../common/macros.h"
#include "../../common/protocol.h"
#include "../../common/queue.h"
#include "../../common/thread.h"

class Sender final : public Thread {
    Queue<dto::Response> responses;
    ProtocolSender& sender;
    spdlog::logger* log;

    friend class ClientHandler;
    void run() override;
    void stop() override;

   public:
    explicit Sender(ProtocolSender&, spdlog::logger*);

    MAKE_FIXED(Sender)

    void send(const dto_session::SearchResponse&);
    void send(const dto_session::JoinResponse&);
    void send(const dto_session::LeaveResponse&);
    void send(const dto_lobby::StartResponse&);
    void send(const dto::ErrorResponse&);
};
