#pragma once

#include <common/emitter.h>

#include <string>
#include <vector>

#include "common/dto/dto.h"
#include "common/macros.h"
#include "common/protocol.h"
#include "common/thread.h"

class Receiver final : public Thread {
    ProtocolReceiver& receiver;

    friend class Connexion;
    void run() override;
    void stop() override;

   public:
    explicit Receiver(ProtocolReceiver& receiver);

    MAKE_FIXED(Receiver)

    struct Listener : common::Listener<Receiver::Listener> {
        virtual void on_error_response(const std::string&) {}
        virtual void on_search_response(const std::vector<SessionInfo>&) {}
        virtual void on_static_session_data_response(const StaticSessionData&) {
        }
        virtual void on_join_response(const SessionInfo&,
                                      const std::vector<CarInfo>&) {}
        virtual void on_leave_response() {}
        virtual void on_start_game(const CityInfo&, const RaceInfo&) {}
        virtual void on_session_snapshot(const SessionConfig&,
                                         const std::vector<PlayerInfo>&) {}

        virtual void on_game_static_snapshot(const RaceInfo&) {}
        /**
         * This method is called to process a snapshot of the current game
         * state.
         *
         * @param timeElapsed The time elapsed in seconds since the last
         * snapshot.
         * @param playerSnapshots A vector containing the current state of all
         * players, including their positions, speeds, and other relevant
         * information.
         */
        virtual void on_game_snapshot(const GameSnapshot&) {}

        virtual void on_collision_event(const CollisionEvent&) {}

        ~Listener() override = default;

       protected:
        void subscribe(Receiver&);
    };

   private:
    common::Emitter<Receiver::Listener> emitter;

    void recv(const dto::ErrorResponse&);
    void recv(const dto_search::SearchResponse&);
    void recv(const dto_search::StaticSessionDataResponse&);
    void recv(const dto_search::JoinResponse&);
    void recv(const dto_session::LeaveResponse&);
    void recv(const dto_session::StartResponse&);
    void recv(const dto_session::SessionSnapshot&);
    void recv(const dto_game::GameStaticSnapshot&);
    void recv(const dto_game::GameSnapshotPacket&);
    void recv(const dto_game::EventPacket&);
};
