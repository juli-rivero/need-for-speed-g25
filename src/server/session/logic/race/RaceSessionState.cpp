#include "RaceSession.h"

void RaceSession::updateCountdown() {
    if (_raceClock >= countdownTime) {
        _state = RaceState::Running;
        _raceClock = 0.0f;
    }
}

void RaceSession::updateRunning(float dt) {
    bool everyoneDone = true;

    for (auto* p : players) {
        if (!p->isFinished() && !p->isDQ()) {
            p->tickTime(dt);
            everyoneDone = false;
        }
    }

    if (_raceClock >= cfg.getRaceTimeLimitSec()) {
        for (auto* p : players)
            if (!p->isFinished() && !p->isDQ()) p->markDQ();

        _state = RaceState::Finished;
        return;
    }

    if (everyoneDone) _state = RaceState::Finished;
}
