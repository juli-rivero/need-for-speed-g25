#include "client/game/assets.h"

#include <memory>

const std::unordered_map<CityName, std::string> AssetsScreen::CITY_PATH = {
    {"LibertyCity", "assets/cities/liberty_city.png"},
    {"SanAndreas", "assets/cities/san_andreas.png"},
    {"ViceCity", "assets/cities/vice_city.png"}};

const std::unordered_map<CityName, std::string> AssetsSound::CITY_PATH = {
    {"LibertyCity", "assets/sound/music_libertycity.mp3"},
    {"SanAndreas", "assets/sound/music_sanandreas.mp3"},
    {"ViceCity", "assets/sound/music_vicecity.mp3"}};

AssetsScreen::AssetsScreen(SDL2pp::Renderer& renderer,
                           const CityName& city_name)
    : car_speedster(renderer, "assets/cars/speedster.png"),
      car_tank(renderer, "assets/cars/tank.png"),
      car_drifter(renderer, "assets/cars/drifter.png"),
      car_rocket(renderer, "assets/cars/rocket.png"),
      car_classic(renderer, "assets/cars/classic.png"),
      car_offroad(renderer, "assets/cars/offroad.png"),
      car_ghost(renderer, "assets/cars/ghost.png"),
      font("assets/fonts/OpenSans-Regular.ttf", 20),
      arrow(renderer, "assets/misc/arrow.png"),
      white(renderer, "assets/misc/white.png") {
    car_name.insert({CarType::Speedster, &car_speedster});
    car_name.insert({CarType::Tank, &car_tank});
    car_name.insert({CarType::Drifter, &car_drifter});
    car_name.insert({CarType::Rocket, &car_rocket});
    car_name.insert({CarType::Classic, &car_classic});
    car_name.insert({CarType::Offroad, &car_offroad});
    car_name.insert({CarType::Ghost, &car_ghost});

    try {
        city = std::make_unique<SDL2pp::Texture>(renderer,
                                                 CITY_PATH.at(city_name));
    } catch (...) {
        throw std::runtime_error("No se pudo abrir el mapa " + city_name);
    }
}

AssetsSound::AssetsSound(const CityName& city_name)
    : brake("assets/sound/brake.ogg"),
      crash("assets/sound/crash.ogg"),
      checkpoint("assets/sound/checkpoint.ogg"),
      goal("assets/sound/goal.ogg") {
    try {
        music = std::make_unique<SDL2pp::Music>(CITY_PATH.at(city_name));
    } catch (...) {
        throw std::runtime_error("No se pudo abrir la musica de " + city_name);
    }
}
