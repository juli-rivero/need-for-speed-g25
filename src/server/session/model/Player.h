#ifndef PLAYER_H
#define PLAYER_H

#include <string>
#include <memory>
#include "Car.h"
#include "../logic/NetworkTypes.h"
//TODO: PODRIA ser neceario capaz para el networking
class Player {
private:
    PlayerId id;
    std::string name;
    std::shared_ptr<Car> car;             // su auto actual
    RaceProgressSnapshot raceProgress;    // checkpoint actual, flags, etc.

public:
    Player(PlayerId id, const std::string& name, std::shared_ptr<Car> car)
        : id(id), name(name), car(std::move(car)) {}

    PlayerId getId() const { return id; }
    const std::string& getName() const { return name; }
    std::shared_ptr<Car> getCar() const { return car; }

    void setRaceProgress(const RaceProgressSnapshot& rp) { raceProgress = rp; }
    const RaceProgressSnapshot& getRaceProgress() const { return raceProgress; }

    bool isAlive() const { return car && car->getHealth() > 0; }
};

#endif
