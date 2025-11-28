#include <ranges>

#include "MatchSession.h"

WorldSnapshot MatchSession::getSnapshot() const {
    WorldSnapshot snap;

    snap.time = _world.getTime();
    snap.matchState = _state;
    snap.currentRaceIndex = _currentRace;
    snap.raceState = _race->getState();
    snap.raceElapsed = _race->elapsedRaceTime();
    snap.raceCountdown = _race->countdownRemaining();
    snap.raceTimeLeft =
        std::max(0.0f, _cfg.getRaceTimeLimitSec() - _race->elapsedRaceTime());

    for (const auto& p : _players | std::views::values)
        snap.players.push_back(p->buildSnapshot());

    if (_traffic)
        for (auto& n : _traffic->getNPCs())
            snap.npcs.push_back(n->buildSnapshot());

    return snap;
}

CityInfo MatchSession::getCityInfo() const {
    std::vector<Bound> walls, railings;

    for (const auto& b : _buildings) {
        auto [x, y] = b->getPosition();
        Bound bd{Point{x, y}, b->getWidth(), b->getHeight()};

        if (b->getEntityType() == EntityType::Wall) walls.push_back(bd);
        if (b->getEntityType() == EntityType::Railing) railings.push_back(bd);
    }

    return {.name = _raceFiles[_currentRace],
            .walls = walls,
            .bridges = _bridges,
            .railings = railings,
            .overpasses = _overpasses};
}

RaceInfo MatchSession::getRaceInfo() const {
    RaceInfo info;
    info.name = _raceFiles[_currentRace];

    for (const auto& cp : _race->getCheckpoints()) {
        auto [x, y] = cp->getPosition();
        info.checkpoints.push_back(CheckpointInfo{
            .order = static_cast<uint32_t>(cp->getOrder()),
            .bound = {Point{x, y}, cp->getWidth(), cp->getHeight()},
            .angle = cp->getAngle(),
            .type = cp->getType(),
        });
    }

    for (auto& sp : _race->getSpawnPoints())
        info.spawnPoints.push_back({{sp.x, sp.y}, sp.angle});

    return info;
}
