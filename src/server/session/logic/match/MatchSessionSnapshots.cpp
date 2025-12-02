#include <ranges>

#include "MatchSession.h"

std::vector<PlayerId> MatchSession::computeRacePositions(
    const std::vector<PlayerSnapshot>& players) const {
    struct Entry {
        PlayerId id;
        uint32_t checkpoint;
        float distToNext;
        float netTime;
    };

    std::vector<Entry> arr;

    for (const auto& player : players) {
        const PlayerId id = player.id;
        const auto& progress = player.raceProgress;
        float dist = 999999.f;

        if (auto nextCP = _race->nextCheckpointFor(id)) {
            const auto& [cx, cy] = nextCP.value()->getPosition();
            const auto& [px, py] = player.car.bound.pos;
            float dx = px - cx;
            float dy = py - cy;
            dist = std::sqrt(dx * dx + dy * dy);
        }

        float net = progress.elapsed + progress.penaltyTime;

        arr.push_back({id, progress.nextCheckpoint, dist, net});
    }

    std::ranges::sort(arr, [](const auto& a, const auto& b) {
        if (a.checkpoint != b.checkpoint) return a.checkpoint > b.checkpoint;
        if (a.distToNext != b.distToNext) return a.distToNext < b.distToNext;
        return a.netTime < b.netTime;
    });

    std::vector<PlayerId> result;
    for (auto& e : arr) result.push_back(e.id);

    return result;
}

std::vector<PlayerId> MatchSession::computeMatchPositions(
    std::vector<PlayerSnapshot>& players) const {
    std::vector<PlayerId> result;
    result.reserve(players.size());
    std::ranges::sort(players, [](const auto& a, const auto& b) {
        if (a.raceProgress.disqualified and not b.raceProgress.disqualified)
            return true;
        if (not a.raceProgress.disqualified and b.raceProgress.disqualified)
            return false;
        return a.netTime < b.netTime;
    });
    for (const auto& p : players) result.push_back(p.id);
    return result;
}

GameSnapshot MatchSession::getSnapshot() const {
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
    const MatchSnapshot match{
        .matchState = _state,
        .currentRaceIndex = static_cast<uint32_t>(_currentRace),
        .time = _world.getTime(),
        .positions = computeMatchPositions(players),
    };
    const RaceSnapshot race{
        .raceState = _race->getState(),
        .raceElapsed = _race->elapsedRaceTime(),
        .raceCountdown = _race->countdownRemaining(),
        .raceTimeLeft = std::max(
            0.0f, _cfg.getRaceTimeLimitSec() - _race->elapsedRaceTime()),
        .positions = computeRacePositions(players),
    };

    return GameSnapshot{
        .players = players,
        .npcs = npcs,
        .match = match,
        .race = race,
    };
}

RaceInfo MatchSession::getRaceInfo() const {
    std::vector<CheckpointInfo> checkpoint_infos;
    for (const auto& cp : _race->getCheckpoints()) {
        auto [x, y] = cp->getPosition();
        checkpoint_infos.push_back(CheckpointInfo{
            .order = static_cast<uint32_t>(cp->getOrder()),
            .bound = {Point{x, y}, cp->getWidth(), cp->getHeight()},
            .angle = cp->getAngle(),
            .type = cp->getType(),
        });
    }

    return RaceInfo{
        .name = _raceFiles[_currentRace],
        .checkpoints = checkpoint_infos,
        .spawnPoints = _race->getSpawnPoints(),
    };
}
