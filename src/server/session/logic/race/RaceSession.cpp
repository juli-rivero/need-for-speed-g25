#include "RaceSession.h"

RaceSession::RaceSession(const YamlGameConfig& cfg,
                         std::vector<std::unique_ptr<Checkpoint>>&& checkpoints,
                         std::vector<SpawnPointInfo>&& spawnPoints,
                         std::vector<Player*>&& players)
    : cfg(cfg),
      players(std::move(players)),
      checkpoints(std::move(checkpoints)),
      spawnPoints(std::move(spawnPoints)) {
    setPlayersPositions();
}

void RaceSession::setPlayersPositions() {
    for (size_t i = 0; i < players.size() && i < spawnPoints.size(); ++i) {
        const auto& player = players[i];
        const auto& spawn = spawnPoints[i];

        player->getCar()->setTransform(spawn.pos, spawn.angle);
    }
}

void RaceSession::start() {
    _state = RaceState::Countdown;
    _raceClock = 0.0f;

    for (auto* p : players) p->resetRaceState();
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
