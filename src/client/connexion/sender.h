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
    virtual void request_static_session_data() = 0;
    virtual void request_create_and_join_session(const SessionConfig&) = 0;
    virtual void request_leave_current_session() = 0;
    virtual void set_ready(bool ready) = 0;
    virtual void choose_car(const CarType&) = 0;

    virtual void start_turning(TurnDirection) = 0;
    virtual void stop_turning(TurnDirection) = 0;
    virtual void start_accelerating() = 0;
    virtual void stop_accelerating() = 0;
    virtual void start_breaking() = 0;
    virtual void stop_breaking() = 0;
    virtual void start_using_nitro() = 0;

    virtual ~Api() = default;
};

class Sender final : public Thread, public Api {
    Queue<dto::Request> requests;
    ProtocolSender& sender;

    friend class Connexion;
    void run() override;
    void stop() override;

   public:
    explicit Sender(ProtocolSender&);

    MAKE_FIXED(Sender)

    void request_search_all_sessions() override;
    void request_join_session(const std::string&) override;
    void request_static_session_data() override;
    void request_create_and_join_session(const SessionConfig&) override;
    void request_leave_current_session() override;
    void set_ready(bool ready) override;
    void choose_car(const CarType&) override;

    // Game
    void start_turning(TurnDirection) override;
    void stop_turning(TurnDirection) override;
    void start_accelerating() override;
    void stop_accelerating() override;
    void start_breaking() override;
    void stop_breaking() override;
    void start_using_nitro() override;
};
