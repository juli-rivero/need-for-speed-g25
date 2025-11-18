#pragma once

#include <memory>
#include <unordered_map>
#include <utility>
#include <vector>

#include "common/emitter.h"
#include "common/queue.h"
#include "common/thread.h"
#include "server/config/YamlGameConfig.h"
#include "server/session/logic/MatchSession.h"
#include "server/session/physics/Box2DPhysicsWorld.h"

class GameSessionFacade : public Thread {
   private:
    Box2DPhysicsWorld world;
    const YamlGameConfig& config;
    std::unique_ptr<MatchSession> match;
    std::unordered_map<PlayerId, PlayerInput> inputStates;
    using Thread::start;

    Queue<std::pair<PlayerId, CarInput>> queue_actions;

   public:
    void start(const std::vector<RaceDefinition>& races,
               const std::vector<PlayerConfig>& players);

    void run() override;
    void stop() override;

    explicit GameSessionFacade(const YamlGameConfig& configPath);

    struct Listener : common::Listener<GameSessionFacade::Listener> {
        virtual void on_snapshot(const WorldSnapshot&) = 0;

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

   private:
    common::Emitter<GameSessionFacade::Listener> emitter;
};
