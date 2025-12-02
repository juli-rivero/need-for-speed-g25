#pragma once

#include <server/session/logic/match/MatchSession.h>

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

class MatchSessionBuilder {
    EntityFactory factory;
    Box2DPhysicsWorld& _world;
    const YamlGameConfig& _cfg;

    template <typename T>
    using u_ptr = std::unique_ptr<T>;
    template <typename K, typename V>
    using u_map = std::unordered_map<K, V>;

    u_map<PlayerId, u_ptr<Player>> _players;
    std::vector<std::string> _raceFiles;
    std::vector<u_ptr<Wall>> _buildings;
    std::vector<u_ptr<BridgeSensor>> _sensors;
    RoadGraph _roadGraph;

   public:
    MatchSessionBuilder(const YamlGameConfig& cfg, Box2DPhysicsWorld& world);

    MatchSessionBuilder& setPlayers(const std::vector<PlayerConfig>& players);
    MatchSessionBuilder& setRaceFiles(
        const std::vector<std::string>& raceFiles);
    MatchSessionBuilder& setBuildings(const std::vector<Bound>& walls,
                                      const std::vector<Bound>& railings);
    MatchSessionBuilder& setSensors(const std::vector<Bound>& upper,
                                    const std::vector<Bound>& lower);
    MatchSessionBuilder& setRoads(const std::vector<RoadShape>& roads);
    MatchSession build();
};
