#ifndef PLAYER_H
#define PLAYER_H

#include <string>
#include <memory>
#include "Car.h"

//TODO: PODRIA ser neceario capaz para el networking
class Player {
private:
    int id;
    std::string name;
    std::shared_ptr<Car> car;

public:
    Player(int id, const std::string& name)
        : id(id), name(name) {}

    ~Player() = default;

    int getId() const { return id; }
    const std::string& getName() const { return name; }

    void assignCar(const std::shared_ptr<Car>& c) { car = c; }
    std::shared_ptr<Car> getCar() const { return car; }
};

#endif
