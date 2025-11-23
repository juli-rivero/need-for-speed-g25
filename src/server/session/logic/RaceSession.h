#pragma once

#include <algorithm>
#include <memory>
#include <optional>
#include <unordered_map>
#include <utility>
#include <vector>

#include "../../config/YamlGameConfig.h"
#include "../model/Car.h"
#include "../model/Checkpoint.h"
#include "NetworkTypes.h"
#include "server/session/logic/types.h"
#include "server/session/model/BridgeSensor.h"
#include "server/session/model/Player.h"

struct SpawnPoint;
class RaceSession {
   public:
    // Player ya no es ownership de Racesession
    RaceSession(const YamlGameConfig& cfg, CityId city,
                std::vector<std::unique_ptr<Checkpoint>> checkpoints,
                std::vector<SpawnPoint> spawnPoints,
                const std::vector<Player*>& players);
    void start();
    void update(float dt);

    bool isFinished() const { return _state == RaceState::Finished; }
    RaceState getState() const { return _state; }

    float elapsedRaceTime() const { return _raceClock; }
    float countdownRemaining() const;

    const std::vector<std::unique_ptr<Checkpoint>>& getCheckpoints() const {
        return checkpoints;
    }
    const std::vector<SpawnPoint>& getSpawnPoints() const {
        return spawnPoints;
    }

    void onCheckpointCrossed(PlayerId player, int checkpointOrder) const;
    void onCarDestroyed(PlayerId player);

    std::optional<const Checkpoint*> nextCheckpointFor(PlayerId p) const;

    std::vector<PlayerResult> makeResults() const;

   private:
    const YamlGameConfig& cfg;
    CityId city;

    RaceState _state{RaceState::Countdown};
    float _raceClock{0.0f};
    float countdownTime{4.0f};  // segundos de cuenta regresiva

    std::vector<Player*> players;
    std::vector<std::unique_ptr<Checkpoint>> checkpoints;
    std::vector<SpawnPoint> spawnPoints;

#if OFFLINE
    std::vector<std::unique_ptr<BridgeSensor>> _sensors;

   public:
    const std::vector<std::unique_ptr<BridgeSensor>>& getSensors() const {
        return _sensors;
    }
    void setSensors(std::vector<std::unique_ptr<BridgeSensor>> sensors) {
        _sensors = std::move(sensors);
    }
#endif
    void orderCheckpointsByOrder();
};
