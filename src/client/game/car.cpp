#include "client/game/car.h"

BaseCar::BaseCar(const Bound& bound, float angle, CarType type,
                 RenderLayer layer)
    : pos(bound, angle, true), type(type), layer(layer) {}

PlayerCar::PlayerCar(const PlayerSnapshot& snapshot)
    : BaseCar(snapshot.car.bound, snapshot.car.angle, snapshot.car.type,
              snapshot.car.layer),
      name(snapshot.name),
      next_checkpoint(snapshot.raceProgress.nextCheckpoint),
      finished(snapshot.raceProgress.finished),
      disqualified(snapshot.raceProgress.disqualified ||
                   (snapshot.car.health <= 0)),
      braking(snapshot.car.braking),
      health(snapshot.car.health),
      speed(snapshot.car.speed),
      id(snapshot.id),
      time(snapshot.raceProgress.elapsedTime),
      nitro_active(snapshot.car.nitroActive) {}

NpcCar::NpcCar(const NpcSnapshot& snapshot)
    : BaseCar(snapshot.bound, snapshot.angle, snapshot.type, snapshot.layer) {}
