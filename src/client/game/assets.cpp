#include "client/game/classes.h"

Assets::Assets(SDL2pp::Renderer& renderer)
    : car_speedster(renderer, "assets/cars/speedster.png"),
      car_tank(renderer, "assets/cars/tank.png"),
      car_drifter(renderer, "assets/cars/drifter.png"),
      car_rocket(renderer, "assets/cars/rocket.png"),
      car_classic(renderer, "assets/cars/classic.png"),
      car_offroad(renderer, "assets/cars/offroad.png"),
      car_ghost(renderer, "assets/cars/ghost.png"),
      city_liberty(renderer, "assets/cities/liberty_city.png"),
      sound_brake("assets/sound/brake.ogg"),
      sound_crash("assets/sound/crash.ogg"),
      sound_finish("assets/sound/finish.ogg"),
      font("assets/fonts/OpenSans-Regular.ttf", 20),
      arrow(renderer, "assets/misc/arrow.png") {
    car_name.insert({CarType::Speedster, &car_speedster});
    car_name.insert({CarType::Tank, &car_tank});
    car_name.insert({CarType::Drifter, &car_drifter});
    car_name.insert({CarType::Rocket, &car_rocket});
    car_name.insert({CarType::Classic, &car_classic});
    car_name.insert({CarType::Offroad, &car_offroad});
    car_name.insert({CarType::Ghost, &car_ghost});

    city_name.insert({"LibertyCity", &city_liberty});
    // TODO(franco): sumar mapeos a otras ciudades
}
