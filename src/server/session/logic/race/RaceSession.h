#pragma once

#include <algorithm>
#include <memory>
#include <optional>
#include <utility>
#include <vector>

#include "server/config/YamlGameConfig.h"
#include "server/session/model/Car.h"
#include "server/session/model/Checkpoint.h"
#include "server/session/model/Player.h"

class RaceSession {
   public:
    // Player ya no es ownership de Racesession
    RaceSession(const YamlGameConfig& cfg,
                std::vector<std::unique_ptr<Checkpoint>>&& checkpoints,
                std::vector<SpawnPointInfo>&& spawnPoints,
                std::vector<Player*>&& players);
    void setPlayersPositions();

    void start();
    void update(float dt);

    bool isFinished() const { return _state == RaceState::Finished; }
    RaceState getState() const { return _state; }

    float elapsedRaceTime() const { return _raceClock; }

    const std::vector<std::unique_ptr<Checkpoint>>& getCheckpoints() const {
        return checkpoints;
    }
    const std::vector<SpawnPointInfo>& getSpawnPoints() const {
        return spawnPoints;
    }

    void onCheckpointCrossed(PlayerId player, int checkpointOrder) const;
    void onCarDestroyed(PlayerId player);

    std::optional<const Checkpoint*> nextCheckpointFor(PlayerId p) const;

    void updateCountdown();
    void updateRunning(float dt);
    float countdownRemaining() const {
        if (_state != RaceState::Countdown) return 0.f;
        return std::max(0.f, countdownTime - _raceClock);
    }

   private:
    const YamlGameConfig& cfg;

    RaceState _state{RaceState::Countdown};
    float _raceClock{0.0f};
    float countdownTime{4.0f};  // segundos de cuenta regresiva

    std::vector<Player*> players;
    const std::vector<std::unique_ptr<Checkpoint>> checkpoints;
    const std::vector<SpawnPointInfo> spawnPoints;
};
