#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "../../config/YamlGameConfig.h"
#include "../../session/model/Player.h"
#include "../physics/Box2DPhysicsWorld.h"
#include "NetworkTypes.h"
#include "RaceSession.h"
#include "UpgradeSystem.h"
#include "server/session/logic/types.h"
#include "server/session/model/BridgeSensor.h"
#include "server/session/model/Wall.h"

class MatchSession {
   private:
    const YamlGameConfig& _cfg;
    Box2DPhysicsWorld& _world;

    const std::vector<std::string> _raceFiles;  // N carreras planificadas
    std::size_t _currentRace{0};

    const std::vector<PlayerConfig> _playerConfigs;
    std::unordered_map<PlayerId, std::unique_ptr<Player>> _players;

    std::vector<std::unique_ptr<Wall>> _buildings;
    std::vector<Bound> _bridges;
    std::vector<Bound> _overpasses;

    std::unique_ptr<RaceSession> _race;  // carrera en curso
    MatchState _state{MatchState::Starting};
    float _intermissionClock{0.0f};

    UpgradeSystem _upgradeSystem;
    std::unordered_map<PlayerId, std::vector<UpgradeChoice>> _queuedUpgrades;
    std::optional<EndRaceSummaryPacket> pendingEndRacePacket;

    void startRace(std::size_t raceIndex);
    EndRaceSummaryPacket finishRaceAndComputeTotals();
    void startIntermission();
    void endIntermissionAndPrepareNextRace();

   public:
    MatchSession(const YamlGameConfig& cfg, std::vector<std::string> raceFiles,
                 Box2DPhysicsWorld& world, std::vector<PlayerConfig> players);

    void update(float dt);  // delega a la carrera actual / intermission
    void applyInput(PlayerId id, const CarInput&);

    WorldSnapshot getSnapshot() const;
    CityInfo getCityInfo() const;
    RaceInfo getRaceInfo() const;
    // upgrades propuestos por jugadores ( que se aplicarán a la próxima
    // carrera)
    void queueUpgrades(
        const std::unordered_map<PlayerId, std::vector<UpgradeChoice>>& ups);

    MatchState state() const { return _state; }

    const std::vector<std::unique_ptr<Wall>>& getWalls() const {
        return _buildings;
    }
    bool hasPendingEndRacePacket() const {
        return pendingEndRacePacket.has_value();
    }
    EndRaceSummaryPacket consumeEndRacePacket() {
        auto p = *pendingEndRacePacket;
        pendingEndRacePacket.reset();
        return p;
    }
#if OFFLINE
    const std::vector<std::unique_ptr<BridgeSensor>>& getSensors() const {
        return _race->getSensors();
    }
#endif
};
