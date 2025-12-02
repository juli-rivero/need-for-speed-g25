#pragma once

#include <SDL2pp/SDL2pp.hh>
#include <string>

#include "client/game/bounding_box.h"
#include "common/structs.h"

struct BaseCar {
   public:
    const BoundingBox pos;
    const CarType type;
    const RenderLayer layer;

    BaseCar(const Bound& bound, float angle, CarType type, RenderLayer layer);
};

struct PlayerCar : BaseCar {
   public:
    // Atributos utilizados del snapshot
    const std::string& name;
    const uint32_t next_checkpoint;
    const bool finished;
    const bool disqualified;
    const bool braking;
    const int health;
    const int speed;
    const PlayerId id;
    const float time;
    const bool nitro_active;

    explicit PlayerCar(const PlayerSnapshot& snapshot);
};

struct NpcCar : BaseCar {
   public:
    explicit NpcCar(const NpcSnapshot& snapshot);
};
