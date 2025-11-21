#pragma once

#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "../../config/YamlGameConfig.h"
#include "../../session/model/Player.h"
#include "../physics/Box2DPhysicsWorld.h"
#include "NetworkTypes.h"
#include "RaceSession.h"
#include "UpgradeSystem.h"
#include "server/session/logic/types.h"
#include "server/session/model/Bridge.h"
#include "server/session/model/Wall.h"

class MatchSession {
   private:
    const YamlGameConfig& _cfg;
    Box2DPhysicsWorld& _world;
    std::vector<std::unique_ptr<Wall>> _walls;
    std::vector<std::unique_ptr<Bridge>> _bridges;
    const std::vector<PlayerConfig> _playerConfigs;
    std::unordered_map<PlayerId, std::unique_ptr<Player>> _players;
    MatchState _state{MatchState::Starting};
    const std::vector<RaceDefinition> _races;  // N carreras planificadas
    std::size_t _currentRace{0};

    std::unique_ptr<RaceSession> _race;  // carrera en curso
    float _intermissionClock{0.0f};

    std::unordered_map<PlayerId, float> _totalTime;  // acumulado por jugador
    std::unordered_map<PlayerId, std::vector<UpgradeChoice>> _queuedUpgrades;
    std::unordered_map<PlayerId, float> _penaltiesForNextRace;
    std::unordered_set<PlayerId> permanentlyDisqualified;

    std::optional<EndRaceSummaryPacket> pendingEndRacePacket;
    std::vector<PlayerResult> _lastResults;

    UpgradeSystem _upgradeSystem;

    void startRace(std::size_t raceIndex);
    EndRaceSummaryPacket finishRaceAndComputeTotals();
    void startIntermission();
    void endIntermissionAndPrepareNextRace();

   public:
    MatchSession(const YamlGameConfig& cfg,
                 std::vector<RaceDefinition> raceDefs, Box2DPhysicsWorld& world,
                 std::vector<PlayerConfig> players);

    void update(float dt);  // delega a la carrera actual / intermission
    void applyInput(PlayerId id, const CarInput&);
    WorldSnapshot getSnapshot() const;
    StaticSnapshot getStaticSnapshot() const;
    // upgrades propuestos por jugadores (se aplicarán a la próxima carrera)
    void queueUpgrades(
        const std::unordered_map<PlayerId, std::vector<UpgradeChoice>>& ups);

    const std::unordered_map<PlayerId, float>& totals() const {
        return _totalTime;
    }
    MatchState state() const { return _state; }

    // acceso a resultados de la última carrera
    const std::vector<PlayerResult>& lastRaceResults() const {
        return _lastResults;
    }
    const std::vector<std::unique_ptr<Wall>>& getWalls() const {
        return _walls;
    }
    const std::vector<std::unique_ptr<Bridge>>& getBridges() const {
        return _bridges;
    }
    bool hasPendingEndRacePacket() const {
        return pendingEndRacePacket.has_value();
    }
    EndRaceSummaryPacket consumeEndRacePacket() {
        auto p = *pendingEndRacePacket;
        pendingEndRacePacket.reset();
        return p;
    }
};
