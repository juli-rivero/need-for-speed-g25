
#include "GameSessionFacade.h"

#include <chrono>
#include <vector>

#include "common/timer_iterator.h"
#include "server/config/MapLoader.h"
#include "server/session/logic/match/MatchSessionBuilder.h"

using std::chrono_literals::operator""ms;

static MatchSession createMatchSession(
    const YamlGameConfig& cfg, Box2DPhysicsWorld& world,
    const std::vector<std::string>& raceFiles,
    const std::vector<PlayerConfig>& players) {
    const MapLoader loader(raceFiles[0], cfg);

    MatchSessionBuilder matchBuilder(cfg, world);
    matchBuilder.setRaceFiles(raceFiles)
        .setPlayers(players)
        .setBuildings(loader.getWalls(), loader.getRailings())
        .setSensors(loader.getUpperSensors(), loader.getLowerSensors())
        .setRoads(loader.getRoadShapes());
    return matchBuilder.build();
}

static CityInfo loadCityInfo(const std::string& path,
                             const YamlGameConfig& cfg) {
    const MapLoader loader(path, cfg);
    return CityInfo{
        .name = loader.getCityName(),
        .walls = loader.getWalls(),
        .bridges = loader.getBridges(),
        .railings = loader.getRailings(),
        .overpasses = loader.getOverpasses(),
        .upper_sensors = loader.getUpperSensors(),
        .lower_sensors = loader.getLowerSensors(),
    };
}

GameSessionFacade::GameSessionFacade(const YamlGameConfig& cfg,
                                     const std::vector<std::string>& raceFiles,
                                     const std::vector<PlayerConfig>& players,
                                     const std::shared_ptr<spdlog::logger>& log)
    : log(log),
      world(),
      match(createMatchSession(cfg, world, raceFiles, players)),
      cityInfo(loadCityInfo(raceFiles[0], cfg)) {
    this->log->debug("GameSessionFacade created");
}

void GameSessionFacade::run() {
    constexpr std::chrono::duration<double> dt(1.f / 60.f);

    TimerIterator iterator(dt);

    size_t passed_iterations = 0;

    while (should_keep_running()) {
        std::pair<PlayerId, CarInput> input;
        while (queue_actions.try_pop(input)) {
            match.applyInput(input.first, input.second);
        }

        std::pair<PlayerId, Cheat> cheater;
        while (queue_cheats.try_pop(cheater)) {
            match.applyCheat(cheater.first, cheater.second);
        }

        std::pair<PlayerId, UpgradeStat> upgrader;
        while (queue_upgrades.try_pop(upgrader)) {
            match.applyUpgrade(upgrader.first, upgrader.second);
        }

        world.step(passed_iterations * dt.count());
        match.update(passed_iterations * dt.count());

        emitter.dispatch(&Listener::on_snapshot, match.getSnapshot());

        auto& collisionManager = world.getCollisionManager();
        while (collisionManager.hasCollisionEvent()) {
            const CollisionEvent& collision = collisionManager.consumeEvent();
            emitter.dispatch(&Listener::on_collision_event, collision);
        }

        passed_iterations = iterator.next();
    }
}

void GameSessionFacade::Listener::subscribe(GameSessionFacade& g) {
    common::Listener<GameSessionFacade::Listener>::subscribe(g.emitter);
}
void GameSessionFacade::startTurningLeft(PlayerId id) {
    queue_actions.try_push({id, CarInput::StartTurningLeft});
}
void GameSessionFacade::stopTurningLeft(PlayerId id) {
    queue_actions.try_push({id, CarInput::StopTurningLeft});
}
void GameSessionFacade::startTurningRight(PlayerId id) {
    queue_actions.try_push({id, CarInput::StartTurningRight});
}
void GameSessionFacade::stopTurningRight(PlayerId id) {
    queue_actions.try_push({id, CarInput::StopTurningRight});
}
void GameSessionFacade::startAccelerating(PlayerId id) {
    queue_actions.try_push({id, CarInput::StartAccelerating});
}
void GameSessionFacade::stopAccelerating(PlayerId id) {
    queue_actions.try_push({id, CarInput::StopAccelerating});
}
void GameSessionFacade::startReversing(PlayerId id) {
    queue_actions.try_push({id, CarInput::StartReversing});
}
void GameSessionFacade::stopReversing(PlayerId id) {
    queue_actions.try_push({id, CarInput::StopReversing});
}
void GameSessionFacade::useNitro(PlayerId id) {
    queue_actions.try_push({id, CarInput::StartUsingNitro});
}
void GameSessionFacade::cheat(PlayerId id, Cheat cheat) {
    queue_cheats.try_push({id, cheat});
}
void GameSessionFacade::upgrade(PlayerId id, UpgradeStat upgrade_stat) {
    queue_upgrades.try_push({id, upgrade_stat});
}
