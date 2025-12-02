#include "MatchSession.h"

void MatchSession::startIntermission() {
    _state = MatchState::Intermission;
    _intermissionClock = 0.0f;
}

void MatchSession::endIntermissionAndPrepareNextRace() {
    auto penalties = _upgradeSystem.applyForNextRace(_queuedUpgrades);

    // aplicar upgrades
    for (auto& [pid, ups] : _queuedUpgrades) {
        auto it = _players.find(pid);
        if (it == _players.end()) continue;

        it->second->setUpgrades(ups);
        it->second->applyUpgrades();
    }

    // aplicar penalidades
    for (auto& [pid, pen] : penalties) {
        auto it = _players.find(pid);
        if (it == _players.end()) continue;

        it->second->resetRaceState(pen);
    }

    _queuedUpgrades.clear();

    if (_currentRace + 1 < _raceFiles.size()) {
        _state = MatchState::Racing;
        startRace(_currentRace + 1);
    } else {
        _state = MatchState::Finished;
    }
}
