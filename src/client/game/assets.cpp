#include "client/game/assets.h"

#include <SDL2pp/SDL2pp.hh>

Assets::Assets(SDL2pp::Renderer& renderer)
    : car1(renderer, "assets/cars/car1.png"),
      city_liberty(renderer, "assets/cities/liberty_city.png"),
      font("assets/fonts/OpenSans-Regular.ttf", 20) {}
