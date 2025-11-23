#pragma once

#include <memory>
#include <utility>
#include <vector>

#include "common/emitter.h"
#include "common/queue.h"
#include "common/thread.h"
#include "server/config/YamlGameConfig.h"
#include "server/session/logic/MatchSession.h"
#include "server/session/model/BridgeSensor.h"
#include "server/session/physics/Box2DPhysicsWorld.h"

class GameSessionFacade : public Thread {
   private:
    Box2DPhysicsWorld world;
    MatchSession match;

    Queue<std::pair<PlayerId, CarInput>> queue_actions;

   public:
    void run() override;

    void stop() override;

    explicit GameSessionFacade(const YamlGameConfig& configPath,
                               const std::vector<RaceDefinition>& races,
                               const std::vector<PlayerConfig>& players);

    struct Listener : common::Listener<GameSessionFacade::Listener> {
        virtual void on_snapshot(const WorldSnapshot&) = 0;
        virtual void on_collision_event(const CollisionEvent&) = 0;

       protected:
        void subscribe(GameSessionFacade&);
    };
#if OFFLINE
    const std::vector<std::unique_ptr<BridgeSensor>>& getDebugSensors() const {
        return match.getSensors();
    }
#endif
    void startTurningLeft(PlayerId id);
    void stopTurningLeft(PlayerId id);
    void startTurningRight(PlayerId id);
    void stopTurningRight(PlayerId id);
    void startAccelerating(PlayerId id);
    void stopAccelerating(PlayerId id);
    void startReversing(PlayerId id);
    void stopReversing(PlayerId id);
    void useNitro(PlayerId id);

#if OFFLINE
    WorldSnapshot getSnapshot() const { return match.getSnapshot(); }
#endif
    StaticSnapshot getStaticSnapshot() const {
        return match.getStaticSnapshot();
    }

   private:
    common::Emitter<GameSessionFacade::Listener> emitter;
};
