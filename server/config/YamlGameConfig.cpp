#include "YamlGameConfig.h"
#include <iostream>

YamlGameConfig::YamlGameConfig(const std::string& filePath) {
      try {
        root = YAML::LoadFile(filePath);
        std::cout << "[YamlGameConfig] Cargando configuración desde: " << filePath << "\n";

        // === Sección "race" ===
        if (root["race"]) {
            const auto& race = root["race"];
            maxPlayers = race["max_players"] ? race["max_players"].as<int>() : maxPlayers;
            timeLimitSec = race["time_limit_sec"] ? race["time_limit_sec"].as<float>() : timeLimitSec;
            intermissionSec = race["intermission_sec"] ? race["intermission_sec"].as<float>() : intermissionSec;
        }

        // === Sección "car_defaults" ===
        if (root["car_defaults"]) {
            const auto& c = root["car_defaults"];
            defaultHealth = c["health"] ? c["health"].as<float>() : defaultHealth;
            defaultMaxSpeed = c["max_speed"] ? c["max_speed"].as<float>() : defaultMaxSpeed;
            defaultAcceleration = c["acceleration"] ? c["acceleration"].as<float>() : defaultAcceleration;
            defaultMass = c["mass"] ? c["mass"].as<float>() : defaultMass;
            defaultControl = c["control"] ? c["control"].as<float>() : defaultControl;
        }

        // === Sección "penalties" ===
        if (root["penalties"]) {
            for (const auto& it : root["penalties"]) {
                penalties[it.first.as<std::string>()] = it.second.as<float>();
            }
        }

        // === Sección "cities" ===
        if (root["cities"]) {
            for (const auto& cityNode : root["cities"]) {
                CityDefinition city;
                city.name = cityNode["city"].as<std::string>();

                if (cityNode["races"]) {
                    for (const auto& raceNode : cityNode["races"]) {
                        RaceDefinition race;
                        race.mapFile = raceNode["map_file"].as<std::string>();
                        city.races.push_back(race);
                    }
                }

                cities.push_back(city);
            }
        }

        printSummary();
    }
    catch (const std::exception& e) {
        std::cerr << " Error cargando config YAML: " << e.what() << std::endl;
        throw;
    }
}
void YamlGameConfig::printSummary() const {
    std::cout << "\n=== CONFIGURACIÓN GLOBAL ===\n";
    std::cout << "Jugadores máx: " << maxPlayers << "\n";
    std::cout << "Límite tiempo: " << timeLimitSec << " s\n";
    std::cout << "Intermission:   " << intermissionSec << " s\n";

    std::cout << "\n=== CAR DEFAULTS ===\n";
    std::cout << "  Health=" << defaultHealth
              << ", MaxSpeed=" << defaultMaxSpeed
              << ", Acc=" << defaultAcceleration
              << ", Mass=" << defaultMass
              << ", Control=" << defaultControl << "\n";

    std::cout << "\n=== PENALIZACIONES ===\n";
    for (const auto& [k, v] : penalties) {
        std::cout << "  " << k << " → +" << v << "s\n";
    }

    std::cout << "\n=== CIUDADES ===\n";
    for (const auto& c : cities) {
        std::cout << " " << c.name << " (" << c.races.size() << " circuitos)\n";
        for (const auto& r : c.races)
            std::cout << "   - " << r.mapFile << "\n";
    }
    std::cout << "=============================\n\n";
}
