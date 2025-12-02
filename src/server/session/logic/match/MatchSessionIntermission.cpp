#include "MatchSession.h"

void MatchSession::startIntermission() {
    _state = MatchState::Intermission;
    _intermissionClock = 0.0f;
}

void MatchSession::endIntermissionAndPrepareNextRace() {
    if (_currentRace + 1 < _raceFiles.size()) {
        _state = MatchState::Racing;
        startRace(_currentRace + 1);
    } else {
        _state = MatchState::Finished;
    }
}
