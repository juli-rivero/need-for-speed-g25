#include "server/session/logic/race/RaceSessionBuilder.h"

#include <algorithm>
#include <ranges>
#include <utility>

RaceSessionBuilder::RaceSessionBuilder(Box2DPhysicsWorld& world,
                                       const YamlGameConfig& cfg)
    : cfg(cfg), factory(world, cfg) {}
RaceSessionBuilder& RaceSessionBuilder::setPlayers(
    const std::unordered_map<PlayerId, std::unique_ptr<Player>>& playersMap) {
    _players.reserve(playersMap.size());
    for (const auto& player : playersMap | std::views::values)
        _players.push_back(player.get());
    return *this;
}
RaceSessionBuilder& RaceSessionBuilder::setCheckpoints(
    const std::vector<CheckpointInfo>& checkpointInfos) {
    _checkpoints.reserve(checkpointInfos.size());
    for (const auto& cp : checkpointInfos)
        _checkpoints.push_back(factory.createCheckpoint(cp));
    std::ranges::sort(_checkpoints, [](const auto& a, const auto& b) {
        return a->getOrder() < b->getOrder();
    });
    return *this;
}
RaceSessionBuilder& RaceSessionBuilder::setSpawnPoints(
    const std::vector<SpawnPointInfo>& spawnInfos) {
    _spawnPoints = spawnInfos;
    return *this;
}
std::unique_ptr<RaceSession> RaceSessionBuilder::build() {
    return std::make_unique<RaceSession>(cfg, std::move(_checkpoints),
                                         std::move(_spawnPoints),
                                         std::move(_players));
}
