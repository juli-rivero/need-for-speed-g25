#include "server/session/logic/match/MatchSessionBuilder.h"

#include <utility>

MatchSessionBuilder::MatchSessionBuilder(const YamlGameConfig& cfg,
                                         Box2DPhysicsWorld& world)
    : factory(world, cfg), _world(world), _cfg(cfg) {}

MatchSessionBuilder& MatchSessionBuilder::setPlayers(
    const std::vector<PlayerConfig>& configs) {
    for (const auto& config : configs) {
        auto car =
            factory.createCar(config.carType, {0, 0}, 0, EntityType::Car);
        _world.getCollisionManager().registerCar(car.get(), config.id);

        _players.emplace(
            config.id,
            std::make_unique<Player>(config.id, config.name, std::move(car)));
    }
    return *this;
}
MatchSessionBuilder& MatchSessionBuilder::setRaceFiles(
    const std::vector<std::string>& raceFiles) {
    _raceFiles = raceFiles;
    return *this;
}
MatchSessionBuilder& MatchSessionBuilder::setBuildings(
    const std::vector<Bound>& walls, const std::vector<Bound>& railings) {
    _buildings.reserve(walls.size() + railings.size());
    for (const auto& wall : walls)
        _buildings.push_back(factory.createBuilding(wall, EntityType::Wall));
    for (const auto& railing : railings)
        _buildings.push_back(
            factory.createBuilding(railing, EntityType::Railing));
    return *this;
}
MatchSessionBuilder& MatchSessionBuilder::setSensors(
    const std::vector<Bound>& uppers, const std::vector<Bound>& lowers) {
    _sensors.reserve(uppers.size() + lowers.size());
    for (const auto& upper : uppers)
        _sensors.push_back(
            factory.createBridgeSensor(RenderLayer::OVER, upper));
    for (const auto& lower : lowers)
        _sensors.push_back(
            factory.createBridgeSensor(RenderLayer::UNDER, lower));
    return *this;
}
MatchSessionBuilder& MatchSessionBuilder::setRoads(
    const std::vector<RoadShape>& roads) {
    for (const auto& r : roads) _roadGraph.addRoad(r);
    _roadGraph.build();
    return *this;
}
MatchSession MatchSessionBuilder::build() {
    return MatchSession(_cfg, _world, std::move(_raceFiles),
                        std::move(_players), std::move(_buildings),
                        std::move(_sensors), std::move(_roadGraph));
}
