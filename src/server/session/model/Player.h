#pragma once

#include <memory>
#include <string>
#include <utility>

#include "../logic/NetworkTypes.h"
#include "Car.h"
#include "server/session/logic/RaceSession.h"

class Player {
    const PlayerId id;
    const std::string name;
    const std::unique_ptr<Car> car;  // su auto actual
    RaceSession const* raceSession;

   public:
    Player(const PlayerId id, const std::string& name,
           std::unique_ptr<Car>&& car)
        : id(id), name(name), car(std::move(car)), raceSession(nullptr) {}

    void setRace(RaceSession const* race) { this->raceSession = race; }

    PlayerSnapshot get_snapshot() const {
        assert(raceSession != nullptr);
        return {.id = id,
                .name = name,
                .car = car->getSnapshot(),
                .raceProgress = raceSession->getProgressForPlayer(id)};
    }

    void update(const float dt) { car->update(dt); }

    bool idDead() const { return car->isDestroyed(); }

    void applyInput(const CarInput input) { car->applyInput(input); }

    void upgradeCar(const UpgradeChoice choice) { car->upgrade(choice); }
};
