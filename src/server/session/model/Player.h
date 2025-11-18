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

   public:
    Player(PlayerId id, const std::string& name, std::shared_ptr<Car> car)
        : id(id), name(name), car(std::move(car)) {}

    bool isAlive() const { return car && car->getHealth() > 0; }
    PlayerId getId() const { return id; }
    const std::string& getName() const { return name; }
    std::shared_ptr<Car> getCar() const { return car; }
};
