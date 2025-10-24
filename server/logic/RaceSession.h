
#ifndef TALLER_TP_RACESESSION_H
#define TALLER_TP_RACESESSION_H


#include <vector>
#include <unordered_map>
#include <memory>
#include <optional>

#include "IGameConfig.h"
#include "types.h"
#include "IRaceEvents.h"

#include "../model/Checkpoint.h"
#include "../model/Hint.h"
#include "../model/Car.h"

class RaceSession : public IRaceEvents {
public:
    enum class State { Countdown, Running, Finished };

    struct PlayerRaceData {
        PlayerId id{};
        std::shared_ptr<Car> car;   // modelo lógico del auto (compartido con GameWorld)
        std::size_t nextCheckpoint{0};
        bool finished{false};
        bool disqualified{false};
        float elapsed{0.0f};        // tiempo crudo
        float penaltyTime{0.0f};    // penalización a aplicar en esta carrera
    };

    RaceSession(const IGameConfig& cfg,
                CityId city,
                std::vector<Checkpoint> checkpoints,
                std::vector<Hint> hints,
                std::vector<std::shared_ptr<Car>> playersCars,
                std::unordered_map<PlayerId, float> initialPenaltiesForThisRace);

    // ciclo
    void start();                 // pasa a Countdown → Running
    void update(float dt);        // reloj global, detección de fin por timeout / llegada total

    // resultados
    bool isFinished() const { return _state == State::Finished; }
    std::vector<PlayerResult> makeResults() const;

    // visibilidad de UI: próximo CP e hints (para "mostrar en orden")
    std::optional<Checkpoint> nextCheckpointFor(PlayerId p) const;
    std::vector<Hint> hintsTowardsNextFor(PlayerId p) const;

    // IRaceEvents (llamados por colisiones / sensores)
    void onCheckpointCrossed(PlayerId player, int checkpointOrder) override;
    void onCarHighImpact(PlayerId player, float impactFactor) override;
    void onCarDestroyed(PlayerId player) override;

    // util
    State state() const { return _state; }
    float elapsedRaceTime() const { return _raceClock; }

private:
    const IGameConfig& _cfg;
    CityId _city;
    State _state{State::Countdown};
    float _raceClock{0.0f};                 // reloj global de la carrera
    float _countdownTime{3.0f};             // opcional: 3s antes de largar

    std::vector<Checkpoint> _checkpoints;   // ordenados por "order"
    std::vector<Hint> _hints;

    std::vector<PlayerRaceData> _players;

    bool everyoneDoneOrDQ() const;
    void applyTimeLimitIfNeeded();
    void orderCheckpointsByOrder(); // asegura orden ascendente por "order"
};

#endif //TALLER_TP_RACESESSION_H
