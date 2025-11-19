#pragma once

#include <string>

#include "common/dto/dto.h"
#include "common/emitter.h"
#include "common/macros.h"
#include "common/protocol.h"
#include "common/thread.h"
#include "spdlog/spdlog.h"

class Receiver final : public Thread {
    ProtocolReceiver& receiver;
    spdlog::logger* log;

    friend class ClientHandler;
    void run() override;
    void stop() override;

   public:
    Receiver(ProtocolReceiver&, spdlog::logger*);

    MAKE_FIXED(Receiver)

    struct Listener : common::Listener<Receiver::Listener> {
        virtual void on_join_request(const std::string&) {}
        virtual void on_search_request() {}
        virtual void on_create_request(const SessionConfig&) {}
        virtual void on_leave_request() {}
        virtual void on_start_request(bool) {}
        virtual void on_choose_car(const std::string&) {}

        virtual void on_request_game_info() {}

        virtual void on_turn_left() {}
        virtual void on_stop_turning_left() {}
        virtual void on_turn_right() {}
        virtual void on_stop_turning_right() {}
        virtual void on_accelerate() {}
        virtual void on_stop_accelerating() {}
        virtual void on_reverse() {}
        virtual void on_stop_reversing() {}
        virtual void on_nitro() {}

        ~Listener() override = default;

       protected:
        void subscribe(Receiver&);
    };

   private:
    common::Emitter<Receiver::Listener> emitter;

    void recv(const dto_search::JoinRequest&);
    void recv(const dto_search::SearchRequest&);
    void recv(const dto_search::CreateRequest&);
    void recv(const dto_session::LeaveRequest&);
    void recv(const dto_session::StartRequest&);
    void recv(const dto_session::ChooseCarRequest&);

    // Game requests
    void recv(const dto_game::TurnRequest&);
    void recv(const dto_game::AccelerateRequest&);
    void recv(const dto_game::UseBoostRequest&);
    void recv(const dto_game::ReverseRequest&);
};
