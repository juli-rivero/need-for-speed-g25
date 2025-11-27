#include "client/game/assets.h"

Assets::Assets(SDL2pp::Renderer& renderer)
    : car1(renderer, "assets/cars/car1.png"),
      car2(renderer, "assets/cars/car2.png"),
      car3(renderer, "assets/cars/car3.png"),
      car4(renderer, "assets/cars/car4.png"),
      car5(renderer, "assets/cars/car5.png"),
      car6(renderer, "assets/cars/car6.png"),
      car7(renderer, "assets/cars/car7.png"),
      city_liberty(renderer, "assets/cities/liberty_city.png"),
      sound_brake("assets/sound/brake.ogg"),
      sound_crash("assets/sound/crash.ogg"),
      sound_finish("assets/sound/finish.ogg"),
      font("assets/fonts/OpenSans-Regular.ttf", 20) {
    car_name.emplace(CarType::Speedster, &car1);
    car_name.emplace(CarType::Tank, &car2);
    car_name.emplace(CarType::Drifter, &car3);
    car_name.emplace(CarType::Rocket, &car4);
    car_name.emplace(CarType::Classic, &car5);
    car_name.emplace(CarType::Offroad, &car6);
    car_name.emplace(CarType::Ghost, &car7);

    city_name.emplace("LibertyCity", &city_liberty);
}
