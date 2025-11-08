#pragma once

#include <memory>
#include <string>
#include <utility>

#include "../logic/NetworkTypes.h"
#include "Car.h"

class Player {
   private:
    PlayerId id;
    std::string name;
    std::shared_ptr<Car> car;  // su auto actual
    // Estado dinámico de carrera
    int nextCheckpoint{0};
    bool finished{false};
    bool disqualified{false};
    float elapsedTime{0.0f};

   public:
    Player(PlayerId id, const std::string& name, std::shared_ptr<Car> car)
        : id(id), name(name), car(std::move(car)) {}
    // === Estado interno ===
    void advanceCheckpoint() { ++nextCheckpoint; }
    void setDisqualified(bool dq) { disqualified = dq; }
    void setFinished(bool fin) { finished = fin; }
    void addElapsed(float dt) { elapsedTime += dt; }

    bool isAlive() const { return car && car->getHealth() > 0; }
    bool hasFinished() const { return finished; }
    bool isDisqualified() const { return disqualified; }

    PlayerId getId() const { return id; }
    const std::string& getName() const { return name; }
    std::shared_ptr<Car> getCar() const { return car; }

    // === Serialización ===
    RaceProgressSnapshot snapshot() const {
        return {id, nextCheckpoint, finished, disqualified, elapsedTime};
    }
};
