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
                              const std::vector<CarInfo>& carTypes) = 0;
    virtual void reply_left() = 0;
    virtual void reply_error(const std::string&) = 0;

    virtual void send_session_snapshot(
        const SessionConfig&, const std::vector<PlayerInfo>& players) = 0;
    virtual void notify_game_started(const std::string& map,
                                     const StaticSnapshot& info) = 0;

    virtual void send_game_static_snapshot(const StaticSnapshot& info) = 0;

    virtual void send_game_snapshot(float raceTimeLeft,
                                    const std::vector<PlayerSnapshot>&) = 0;

    virtual void send_collision_event(const CollisionEvent&) = 0;

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
                      const std::vector<CarInfo>& carTypes) override;
    void reply_left() override;
    void reply_error(const std::string&) override;

    void send_session_snapshot(const SessionConfig&,
                               const std::vector<PlayerInfo>& players) override;
    void notify_game_started(const std::string& map,
                             const StaticSnapshot& info) override;

    void send_game_static_snapshot(const StaticSnapshot& info) override;

    void send_game_snapshot(float raceTimeLeft,
                            const std::vector<PlayerSnapshot>&) override;

    void send_collision_event(const CollisionEvent&) override;
};
