#pragma once

#include <memory>
#include <string>
#include <utility>

#include "Car.h"

class Player {
    const PlayerId id;
    const std::string name;
    const std::unique_ptr<Car> car;  // su auto actual
    RaceProgressSnapshot raceState{};
    float totalPenalty{0.0f};
    float totalAccumulated{0.0f};

   public:
    Player(const PlayerId id, const std::string& name,
           std::unique_ptr<Car>&& car)
        : id(id), name(name), car(std::move(car)) {}

    // ------------------------------
    //  LÓGICA DEL AUTO
    // ------------------------------
    void applyInput(CarInput input) { car->applyInput(input); }
    void update(float dt) { car->update(dt); }
    Car* getCar() const { return car.get(); }

    // ------------------------------
    //  ESTADO DE CARRERA
    // ------------------------------

    // para reiniciar el estado de carrera para una nueva Race
    void resetRaceState() { raceState = RaceProgressSnapshot{}; }

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
    bool isDQ() const { return raceState.disqualified || car->isDestroyed(); }
    std::size_t getNextCheckpoint() const { return raceState.nextCheckpoint; }

    // ------------------------------
    //  TIEMPOS Y PENALIZACIONES
    // ------------------------------
    PlayerId getId() const { return id; }
    void applyUpgrade(UpgradeStat stat, float delta, float penalty) {
        car->upgrade(stat, delta);
        raceState.penaltyTime += penalty;
        totalPenalty += penalty;
    }
    void accumulateTotal() {
        totalAccumulated += raceState.elapsed + raceState.penaltyTime;
    }

    // ------------------------------
    //  SNAPSHOT PARA EL CLIENTE
    // ------------------------------
    PlayerSnapshot buildSnapshot() const {
        PlayerSnapshot ps;
        ps.id = id;
        ps.name = name;
        ps.car = car->getSnapshot();
        ps.upgrades = car->getUpgrades();

        ps.raceProgress = raceState;

        ps.rawTime = totalAccumulated;
        ps.penalty = totalPenalty;
        ps.netTime = ps.rawTime + ps.penalty;
        return ps;
    }
};
