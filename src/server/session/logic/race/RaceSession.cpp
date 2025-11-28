#include "RaceSession.h"

RaceSession::RaceSession(const YamlGameConfig& cfg,
                         std::vector<std::unique_ptr<Checkpoint>> checkpoints,
                         std::vector<SpawnPoint> spawnPoints,
                         const std::vector<Player*>& racePlayers)
    : cfg(cfg),
      players(racePlayers.begin(), racePlayers.end()),
      checkpoints(std::move(checkpoints)),
      spawnPoints(std::move(spawnPoints)) {
    orderCheckpointsByOrder();
}

void RaceSession::start() {
    _state = RaceState::Countdown;
    _raceClock = 0.0f;

    for (auto* p : players) p->resetRaceState(p->getPenalty());
}

void RaceSession::update(float dt) {
    if (_state == RaceState::Finished) return;

    _raceClock += dt;

    if (_state == RaceState::Countdown) {
        updateCountdown();
        return;
    }

    updateRunning(dt);
}
