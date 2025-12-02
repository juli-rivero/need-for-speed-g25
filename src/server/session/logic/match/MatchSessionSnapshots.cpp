#include <ranges>

#include "MatchSession.h"

GameSnapshot MatchSession::getSnapshot() const {
    const MatchSnapshot match{
        .matchState = _state,
        .currentRaceIndex = static_cast<uint32_t>(_currentRace),
        .time = _world.getTime(),
    };
    const RaceSnapshot race{
        .raceState = _race->getState(),
        .raceElapsed = _race->elapsedRaceTime(),
        .raceCountdown = _race->countdownRemaining(),
        .raceTimeLeft = std::max(
            0.0f, _cfg.getRaceTimeLimitSec() - _race->elapsedRaceTime()),
    };

    std::vector<PlayerSnapshot> players;
    players.reserve(_players.size());
    for (const auto& p : _players | std::views::values)
        players.push_back(p->buildSnapshot());

    std::vector<NpcSnapshot> npcs;
    if (_traffic) {
        const auto& logic_npcs = _traffic->getNPCs();
        npcs.reserve(logic_npcs.size());
        for (auto& npc_logic : logic_npcs)
            npcs.push_back(npc_logic->buildSnapshot());
    }

    return GameSnapshot{
        .match = match,
        .race = race,
        .players = players,
        .npcs = npcs,
    };
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
