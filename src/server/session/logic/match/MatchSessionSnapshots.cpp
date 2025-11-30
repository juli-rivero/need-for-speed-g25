#include <ranges>

#include "MatchSession.h"

std::vector<PlayerId> MatchSession::computePositions() const {
    struct Entry {
        PlayerId id;
        uint32_t checkpoint;
        float distToNext;
        float netTime;
    };

    std::vector<Entry> arr;

    for (auto& [id, player] : _players) {
        const auto snap = player->buildSnapshot();

        uint32_t cp = snap.raceProgress.nextCheckpoint;
        float dist = 999999.f;

        if (auto nextCP = _race->nextCheckpointFor(id)) {
            auto [cx, cy] = nextCP.value()->getPosition();
            float dx = snap.car.bound.pos.x - cx;
            float dy = snap.car.bound.pos.y - cy;
            dist = std::sqrt(dx * dx + dy * dy);
        }

        float net = snap.raceProgress.elapsedTime + player->getPenalty();

        arr.push_back({id, cp, dist, net});
    }

    std::sort(arr.begin(), arr.end(), [](const auto& a, const auto& b) {
        if (a.checkpoint != b.checkpoint) return a.checkpoint > b.checkpoint;
        if (a.distToNext != b.distToNext) return a.distToNext < b.distToNext;
        return a.netTime < b.netTime;
    });

    std::vector<PlayerId> result;
    for (auto& e : arr) result.push_back(e.id);

    return result;
}

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
        .positionsOrdered = computePositions(),
        .matchSummary = getFinalSummary()  // TODO(juli)
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

FinalMatchSummary MatchSession::makeFinalSummary() const {
    std::vector<FinalPlayerResult> results;

    for (auto& [id, player] : _players) {
        FinalPlayerResult r;
        r.id = id;
        r.name = player->getName();
        r.carType = player->getCar()->getType();
        r.rawTime = player->getTotalAccumulated();
        r.penalty = player->getPenalty();
        r.netTime = r.rawTime + r.penalty;

        results.push_back(r);
    }

    // ASCENDENTE (menor tiempo = mejor)
    std::sort(results.begin(), results.end(), [](const auto& a, const auto& b) {
        return a.netTime < b.netTime;
    });
    for (int i = 0; i < static_cast<int>(results.size()); i++)
        results[i].position = i + 1;

    FinalMatchSummary summary;
    summary.players = results;
    summary.winner = results.front().id;

    return summary;
}
