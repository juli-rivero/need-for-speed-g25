#pragma once

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "common/emitter.h"
#include "common/queue.h"
#include "common/thread.h"
#include "match/MatchSession.h"
#include "server/config/YamlGameConfig.h"
#include "server/session/model/BridgeSensor.h"
#include "server/session/physics/Box2DPhysicsWorld.h"

class GameSessionFacade : public Thread {
   private:
    Box2DPhysicsWorld world;
    MatchSession match;

    Queue<std::pair<PlayerId, CarInput>> queue_actions;

   public:
    void run() override;

    explicit GameSessionFacade(const YamlGameConfig& configPath,
                               const std::vector<std::string>& raceFiles,
                               const std::vector<PlayerConfig>& players);

    struct Listener : common::Listener<GameSessionFacade::Listener> {
        virtual void on_snapshot(const WorldSnapshot&) = 0;
        virtual void on_collision_event(const CollisionEvent&) = 0;

       protected:
        void subscribe(GameSessionFacade&);
    };
    // TODO(elvis): BORRAR ESTO Y DEMAS AL FINALIZAR, EL CLIENTE NO RENDERIZA
    // SENSORES, NI LOS NECESITA, SOLO EXISTE EN EL WORLD,usado en modo offline
    const std::vector<std::unique_ptr<BridgeSensor>>& getDebugSensors() const {
        return match.getSensors();
    }

    void startTurningLeft(PlayerId id);
    void stopTurningLeft(PlayerId id);
    void startTurningRight(PlayerId id);
    void stopTurningRight(PlayerId id);
    void startAccelerating(PlayerId id);
    void stopAccelerating(PlayerId id);
    void startReversing(PlayerId id);
    void stopReversing(PlayerId id);
    void useNitro(PlayerId id);

    WorldSnapshot getSnapshot() const { return match.getSnapshot(); }
    CityInfo getCityInfo() const { return match.getCityInfo(); }
    RaceInfo getRaceInfo() const { return match.getRaceInfo(); }

   private:
    common::Emitter<GameSessionFacade::Listener> emitter;
};
