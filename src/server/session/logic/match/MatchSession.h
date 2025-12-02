#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "server/config/YamlGameConfig.h"
#include "server/session/NPC/TrafficSystem.h"
#include "server/session/logic/race/RaceSession.h"
#include "server/session/logic/types.h"
#include "server/session/model/BridgeSensor.h"
#include "server/session/model/Player.h"
#include "server/session/model/Wall.h"
#include "server/session/physics/Box2DPhysicsWorld.h"

class MatchSession {
   private:
    const YamlGameConfig& _cfg;
    Box2DPhysicsWorld& _world;

    std::unique_ptr<TrafficSystem> _traffic;
    const RoadGraph _roadGraph;

    const std::vector<std::string> _raceFiles;  // N carreras planificadas
    std::size_t _currentRace{0};

    template <typename T>
    using u_ptr = std::unique_ptr<T>;
    template <typename K, typename V>
    using u_map = std::unordered_map<K, V>;

    const u_map<PlayerId, u_ptr<Player>> _players;

    const std::vector<u_ptr<Wall>> _buildings;
    const std::vector<u_ptr<BridgeSensor>> _sensors;

    std::unique_ptr<RaceSession> _race;  // carrera en curso
    MatchState _state{MatchState::Starting};
    float _intermissionClock{0.0f};

    void startRace(std::size_t raceIndex);
    void startIntermission();
    void endIntermissionAndPrepareNextRace();

   public:
    MatchSession() = delete;
    MatchSession(const YamlGameConfig& cfg, Box2DPhysicsWorld& world,
                 std::vector<std::string>&& raceFiles,
                 u_map<PlayerId, u_ptr<Player>>&& players,
                 std::vector<u_ptr<Wall>>&& buildings,
                 std::vector<u_ptr<BridgeSensor>>&& sensors,
                 RoadGraph&& roadGraph);

    void update(float dt);  // delega a la carrera actual / intermission
    void applyInput(PlayerId id, const CarInput&) const;
    void applyCheat(PlayerId id, Cheat) const;
    void applyUpgrade(PlayerId id, UpgradeStat stat);

    GameSnapshot getSnapshot() const;
    RaceInfo getRaceInfo() const;

    MatchState state() const { return _state; }

    std::vector<PlayerId> computeRacePositions(
        const std::vector<PlayerSnapshot>&) const;
    std::vector<PlayerId> computeMatchPositions(
        std::vector<PlayerSnapshot>&) const;
};
