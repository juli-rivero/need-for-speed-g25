
#ifndef TALLER_TP_MATCHSESSION_H
#define TALLER_TP_MATCHSESSION_H


#include <vector>
#include <unordered_map>
#include <memory>
#include "types.h"
#include "../config/YamlGameConfig.h"
#include "RaceSession.h"
#include "UpgradeSystem.h"


#include "../physics/Box2DPhysicsWorld.h"

enum class State { Lobby, Racing, Intermission, Finished };
class MatchSession {
private:
    const YamlGameConfig& _cfg;
    Box2DPhysicsWorld& _world;

    State _state{State::Lobby};
    std::vector<RaceDefinition> _races;   // N carreras planificadas
    std::size_t _currentRace{0};

    std::unique_ptr<RaceSession> _race;   // carrera en curso
    float _intermissionClock{0.0f};

    std::unordered_map<PlayerId, float> _totalTime; // acumulado por jugador
    std::unordered_map<PlayerId, std::vector<UpgradeChoice>> _queuedUpgrades;
    std::unordered_map<PlayerId, float> _penaltiesForNextRace;

    std::vector<PlayerResult> _lastResults;

    UpgradeSystem _upgradeSystem;

    void startRace(std::size_t raceIndex);
    void finishRaceAndComputeTotals();
    void startIntermission();
    void endIntermissionAndPrepareNextRace();
public:


    MatchSession(const YamlGameConfig& cfg,
                 std::vector<RaceDefinition> raceDefs,
                 Box2DPhysicsWorld& world);

    void start();            // Lobby → Racing (carrera 0)
    void update(float dt);   // delega a la carrera actual / intermission

    // upgrades propuestos por jugadores (se aplicarán a la próxima carrera)
    void queueUpgrades(const std::unordered_map<PlayerId, std::vector<UpgradeChoice>>& ups);

    const std::unordered_map<PlayerId, float>& totals() const { return _totalTime; }
    State state() const { return _state; }

    // acceso a resultados de la última carrera
    const std::vector<PlayerResult>& lastRaceResults() const { return _lastResults; }


};

#endif //TALLER_TP_MATCHSESSION_H