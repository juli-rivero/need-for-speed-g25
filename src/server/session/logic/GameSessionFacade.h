#pragma once

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "common/emitter.h"
#include "common/queue.h"
#include "common/thread.h"
#include "server/config/YamlGameConfig.h"
#include "server/session/logic/match/MatchSession.h"
#include "server/session/physics/Box2DPhysicsWorld.h"
#include "spdlog/sinks/stdout_color_sinks-inl.h"

class GameSessionFacade : public Thread {
   private:
    std::shared_ptr<spdlog::logger> log;

    Box2DPhysicsWorld world;
    MatchSession match;

    const CityInfo cityInfo;

    Queue<std::pair<PlayerId, CarInput>> queue_actions;
    Queue<std::pair<PlayerId, Cheat>> queue_cheats;
    Queue<std::pair<PlayerId, UpgradeStat>> queue_upgrades;

    MatchState last_state{MatchState::Starting};

    void run() override;

   public:
    explicit GameSessionFacade(const YamlGameConfig& cfg,
                               const std::vector<std::string>& raceFiles,
                               const std::vector<PlayerConfig>& players,
                               const std::shared_ptr<spdlog::logger>&);

    struct Listener : common::Listener<GameSessionFacade::Listener> {
        virtual void on_snapshot(const GameSnapshot&) = 0;
        virtual void on_collision_event(const CollisionEvent&) = 0;
        virtual void on_new_race(const RaceInfo&) = 0;

       protected:
        void subscribe(GameSessionFacade&);
    };

    void startTurningLeft(PlayerId id);
    void stopTurningLeft(PlayerId id);
    void startTurningRight(PlayerId id);
    void stopTurningRight(PlayerId id);
    void startAccelerating(PlayerId id);
    void stopAccelerating(PlayerId id);
    void startReversing(PlayerId id);
    void stopReversing(PlayerId id);
    void useNitro(PlayerId id);

    void cheat(PlayerId id, Cheat);
    void upgrade(PlayerId id, UpgradeStat);

    GameSnapshot getSnapshot() const { return match.getSnapshot(); }
    CityInfo getCityInfo() const { return cityInfo; }
    RaceInfo getRaceInfo() const { return match.getRaceInfo(); }

   private:
    common::Emitter<GameSessionFacade::Listener> emitter;
};
