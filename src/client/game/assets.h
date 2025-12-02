#pragma once

#include <SDL2pp/SDL2pp.hh>
#include <memory>
#include <string>
#include <unordered_map>

#include "common/macros.h"
#include "common/structs.h"

class AssetsScreen final {
   private:
    static const std::unordered_map<CityName, std::string> CITY_PATH;

   public:
    AssetsScreen(SDL2pp::Renderer& renderer, const CityName& city);

    // Assets a secas
    SDL2pp::Texture car_speedster;
    SDL2pp::Texture car_tank;
    SDL2pp::Texture car_drifter;
    SDL2pp::Texture car_rocket;
    SDL2pp::Texture car_classic;
    SDL2pp::Texture car_offroad;
    SDL2pp::Texture car_ghost;

    std::unique_ptr<SDL2pp::Texture> city;

    SDL2pp::Font font;

    SDL2pp::Texture arrow;
    SDL2pp::Texture white;
    SDL2pp::Texture explosion;

    // Mapeos
    std::unordered_map<CarType, SDL2pp::Texture*> car_name;

    MAKE_FIXED(AssetsScreen)
};

class AssetsSound final {
   private:
    static const std::unordered_map<CityName, std::string> CITY_PATH;

   public:
    explicit AssetsSound(const CityName& city_name);

    SDL2pp::Chunk brake;
    SDL2pp::Chunk crash;
    SDL2pp::Chunk checkpoint;
    SDL2pp::Chunk explosion;
    SDL2pp::Chunk goal;

    std::unique_ptr<SDL2pp::Music> music;

    MAKE_FIXED(AssetsSound)
};
