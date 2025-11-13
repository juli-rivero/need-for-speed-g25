#pragma once

#include <memory>
#include <optional>
#include <unordered_map>
#include <vector>
#include "NetworkTypes.h"
#include "../../config/YamlGameConfig.h"
#include "../model/Car.h"
#include "../model/Checkpoint.h"
#include "server/session/logic/types.h"

struct SpawnPoint;
struct PlayerRaceData {
    PlayerId id{};
    std::shared_ptr<Car> car;  // modelo lógico del auto (compartido con GameWorld)
    std::size_t nextCheckpoint{0};
    bool finished{false};
    bool disqualified{false};
    float elapsed{0.0f};      // tiempo crudo
    float penaltyTime{0.0f};  // penalización a aplicar en esta carrera
};

class RaceSession {
   public:
    enum class State { Countdown, Running, Finished };

    RaceSession(
        const YamlGameConfig& cfg, CityId city,
        std::vector<std::unique_ptr<Checkpoint>> checkpoints,
        const std::vector<std::shared_ptr<Car>>& playersCars,
        const std::vector<PlayerId>& playerIds,
        std::vector<SpawnPoint> spawn_points,
        std::unordered_map<PlayerId, float> initialPenaltiesForThisRace);

    // ciclo
    void start();           // pasa a Countdown → Running
    void update(float dt);  // reloj global, detección de fin por timeout /
                            // llegada total

    // resultados
    bool isFinished() const { return _state == State::Finished; }
    std::vector<PlayerResult> makeResults() const;

    // visibilidad de UI: próximo CP e hints (para "mostrar en orden")
    std::optional<const Checkpoint*> nextCheckpointFor(PlayerId p) const;

    // IRaceEvents (llamados por colisiones / sensores)
    void onCheckpointCrossed(PlayerId player, int checkpointOrder);
    void onCarDestroyed(PlayerId player);

    // util
    State state() const { return _state; }
    float elapsedRaceTime() const { return _raceClock; }
    const std::vector<std::unique_ptr<Checkpoint>>& getCheckpoints() const {
        return _checkpoints;
    }
    const std::vector<PlayerRaceData>& getPlayerStates() const {
        return _players;
    }
    RaceProgressSnapshot getProgressForPlayer(PlayerId id) const;
    const std::vector<std::shared_ptr<Car>>& getCars() const {
        static std::vector<std::shared_ptr<Car>> carsCache;
        carsCache.clear();
        for (const auto& p : _players) {
            if (p.car) {
                carsCache.push_back(p.car);
            }
        }
        return carsCache;
    }
    const std::vector<SpawnPoint>& getSpawnPoints() const {
        return _spawnPoints;
    }

   private:
    const YamlGameConfig& _cfg;
    CityId _city;
    State _state{State::Countdown};
    float _raceClock{0.0f};      // reloj global de la carrera
    float _countdownTime{3.0f};  // opcional: 3s antes de largar

    std::vector<std::unique_ptr<Checkpoint>> _checkpoints;  // ordenados por "order"
    std::vector<PlayerRaceData> _players;
    std::vector<SpawnPoint> _spawnPoints;

    bool everyoneDoneOrDQ() const;
    void applyTimeLimitIfNeeded();
    void orderCheckpointsByOrder();  // asegura orden ascendente por "order"
};
