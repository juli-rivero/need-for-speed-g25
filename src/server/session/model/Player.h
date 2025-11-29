#pragma once

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "Car.h"
#include "server/session/logic/types.h"
class Player {
    const PlayerId id;
    const std::string name;
    const std::unique_ptr<Car> car;  // su auto actual
    PlayerRaceData raceState;

    float totalAccumulated{0.0f};

    // Upgrades elegidos por el jugador y TODAVIA no aplicados
    std::vector<UpgradeChoice> pendingUpgrades;

   public:
    Player(const PlayerId id, const std::string& name,
           std::unique_ptr<Car>&& car)
        : id(id), name(name), car(std::move(car)) {}

    // ------------------------------
    //  LÓGICA DEL AUTO
    // ------------------------------
    void applyInput(CarInput input) { car->applyInput(input); }
    void update(float dt) { car->update(dt); }
    const Car* getCar() const { return car.get(); }

    // ------------------------------
    //  ESTADO DE CARRERA
    // ------------------------------

    // para reiniciar el estado de carrera para una nueva Race
    void resetRaceState(float initialPenaltyTime = 0.0f) {
        raceState.nextCheckpoint = 0;
        raceState.finished = false;
        raceState.disqualified = false;
        raceState.elapsed = 0.0f;
        raceState.penaltyTime = initialPenaltyTime;
    }

    void tickTime(float dt) {
        if (!raceState.finished && !raceState.disqualified) {
            raceState.elapsed += dt;
        }
    }

    void onCheckpoint(std::size_t order) {
        if (raceState.finished || raceState.disqualified) return;

        if (order == raceState.nextCheckpoint) {
            raceState.nextCheckpoint++;
        }
    }
    void markFinished() { raceState.finished = true; }
    void markDQ() { raceState.disqualified = true; }
    bool isFinished() const { return raceState.finished; }
    bool isDQ() const { return raceState.disqualified; }
    std::size_t getNextCheckpoint() const { return raceState.nextCheckpoint; }

    // ------------------------------
    //  TIEMPOS Y PENALIZACIONES
    // ------------------------------
    void setPenalty(float p) { raceState.penaltyTime = p; }
    float getPenalty() const { return raceState.penaltyTime; }
    PlayerId getId() const { return id; }
    float getRawTime() const { return raceState.elapsed; }
    float getNetTime() const {
        return raceState.elapsed + raceState.penaltyTime;
    }

    void accumulateTotal() { totalAccumulated += getNetTime(); }
    float getTotalAccumulated() const { return totalAccumulated; }

    // ------------------------------
    //  UPGRADES
    // ------------------------------
    void setUpgrades(const std::vector<UpgradeChoice>& ups) {
        pendingUpgrades = ups;
    }
    const std::vector<UpgradeChoice>& getUpgrades() const {
        return pendingUpgrades;
    }
    void applyUpgrades() {
        for (const auto& up : pendingUpgrades) {
            car->upgrade(up);
        }
        pendingUpgrades.clear();
    }

    // ------------------------------
    //  SNAPSHOT PARA EL CLIENTE
    // ------------------------------
    PlayerSnapshot buildSnapshot() const {
        PlayerSnapshot ps;
        ps.id = id;
        ps.name = name;
        ps.car = car->getSnapshot();

        RaceProgressSnapshot rp;
        rp.nextCheckpoint = static_cast<uint32_t>(raceState.nextCheckpoint);
        rp.finished = raceState.finished;
        rp.disqualified = raceState.disqualified;
        rp.elapsedTime = raceState.elapsed;

        ps.raceProgress = rp;
        return ps;
    }
};
