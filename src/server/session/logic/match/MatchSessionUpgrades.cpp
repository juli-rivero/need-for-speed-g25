#include "MatchSession.h"

EndRaceSummaryPacket MatchSession::finishRaceAndComputeTotals() {
    auto results = _race->makeResults();
    EndRaceSummaryPacket summary;
    summary.raceIndex = _currentRace;

    for (auto& r : results) {
        auto it = _players.find(r.id);
        if (it == _players.end()) continue;

        auto& player = it->second;

        player->setPenalty(r.penalty);
        player->accumulateTotal();

        EndRaceUpgradeReport rep;
        rep.id = r.id;
        rep.penaltyTime = r.penalty;

        if (auto itU = _queuedUpgrades.find(r.id); itU != _queuedUpgrades.end())
            rep.upgrades = itU->second;

        summary.results.push_back(rep);
    }
    return summary;
}

void MatchSession::queueUpgrades(
    const std::unordered_map<PlayerId, std::vector<UpgradeChoice>>& ups) {
    _queuedUpgrades = ups;
}
