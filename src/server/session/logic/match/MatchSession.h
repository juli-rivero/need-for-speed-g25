#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "../../../config/YamlGameConfig.h"
#include "../../model/Player.h"
#include "../../physics/Box2DPhysicsWorld.h"
#include "../race/RaceSession.h"
#include "server/session/NPC/TrafficSystem.h"
#include "server/session/logic/types.h"
#include "server/session/model/BridgeSensor.h"
#include "server/session/model/Wall.h"

class MatchSession {
   private:
    const YamlGameConfig& _cfg;
    Box2DPhysicsWorld& _world;

    std::unique_ptr<TrafficSystem> _traffic;
    RoadGraph _roadGraph;

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

    std::optional<FinalMatchSummary>
        _finalSummary;  // TODO(juli): si lo mandas con el dispatch

    void startRace(std::size_t raceIndex);
    void startIntermission();
    void endIntermissionAndPrepareNextRace();

   public:
    MatchSession(const YamlGameConfig& cfg, std::vector<std::string> raceFiles,
                 Box2DPhysicsWorld& world, std::vector<PlayerConfig> players);

    void update(float dt);  // delega a la carrera actual / intermission
    void applyInput(PlayerId id, const CarInput&) const;

    GameSnapshot getSnapshot() const;
    CityInfo getCityInfo() const;
    RaceInfo getRaceInfo() const;

    MatchState state() const { return _state; }

    // TODO(juli)
    bool hasFinalSummary() const { return _finalSummary.has_value(); }
    FinalMatchSummary consumeFinalSummary() {
        auto s = *_finalSummary;
        _finalSummary.reset();
        return s;
    }
    const std::optional<FinalMatchSummary>& getFinalSummary() const {
        return _finalSummary;
    }

    const std::vector<std::unique_ptr<Wall>>& getWalls() const {
        return _buildings;
    }

    const std::vector<std::unique_ptr<BridgeSensor>>& getSensors() const {
        return _race->getSensors();
    }
    void applyUpgrade(PlayerId id, UpgradeStat stat);
    std::vector<PlayerId> computePositions() const;
    static const char* toPenaltyKey(UpgradeStat s);
    FinalMatchSummary makeFinalSummary() const;
};
