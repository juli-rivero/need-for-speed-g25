#include "YamlGameConfig.h"
#include <iostream>

YamlGameConfig::YamlGameConfig(const std::string& filePath) {
    try {
        root = YAML::LoadFile(filePath);
        std::cout << "[YamlGameConfig] Cargando configuración desde: " << filePath << "\n";

        // === Sección "race" ===
        if (root["race"]) {
            const auto& race = root["race"];
            maxPlayers = race["max_players"].as<int>();
            timeLimitSec = race["time_limit_sec"].as<float>();
            intermissionSec = race["intermission_sec"].as<float>();
        } else {
            throw std::runtime_error(" Falta la sección 'race' en config.yaml");
        }

        // === Sección "car_types" ===
        if (root["car_types"]) {
            for (const auto& c : root["car_types"]) {
                CarType type;
                type.name = c["name"].as<std::string>();
                type.description = c["description"] ? c["description"].as<std::string>() : "";
                type.width = c["width"].as<float>();
                type.height = c["height"].as<float>();
                type.maxSpeed = c["max_speed"].as<float>();
                type.acceleration = c["acceleration"].as<float>();
                type.mass = c["mass"].as<float>();
                type.control = c["control"].as<float>();
                type.health = c["health"].as<float>();
                type.nitroMultiplier = c["nitro_multiplier"].as<float>();
                type.nitroDuration = c["nitro_duration"].as<float>();
                type.nitroCooldown = c["nitro_cooldown"].as<float>();
                type.density = c["density"] ? c["density"].as<float>() : 1.0f;
                type.friction = c["friction"] ? c["friction"].as<float>() : 0.8f;
                type.restitution = c["restitution"] ? c["restitution"].as<float>() : 0.1f;
                type.linearDamping = c["linear_damping"] ? c["linear_damping"].as<float>() : 0.3f;
                type.angularDamping = c["angular_damping"] ? c["angular_damping"].as<float>() : 0.8f;

                carTypes.push_back(type);
            }
        } else {
            throw std::runtime_error("Falta la sección 'car_types' en config.yaml");
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
                        race.city = city.name;
                        city.races.push_back(race);
                    }
                }
                cities.push_back(city);
            }
        }

        printSummary();
    }
    catch (const std::exception& e) {
        std::cerr << "Error cargando config YAML: " << e.what() << std::endl;
        throw;
    }
}

void YamlGameConfig::printSummary() const {
    std::cout << "\n=== CONFIGURACIÓN GLOBAL ===\n";
    std::cout << "Jugadores máx: " << maxPlayers << "\n";
    std::cout << "Límite tiempo: " << timeLimitSec << " s\n";
    std::cout << "Intermission:   " << intermissionSec << " s\n";

    std::cout << "\n=== AUTOS DISPONIBLES ===\n";
    for (const auto& c : carTypes) {
        std::cout << c.name << " ("
                  << c.description << ")\n";
        std::cout << "   speed=" << c.maxSpeed
                  << ", acc=" << c.acceleration
                  << ", mass=" << c.mass
                  << ", control=" << c.control
                  << ", hp=" << c.health
                  << ", nitro×" << c.nitroMultiplier << "\n";
    }

    std::cout << "\n=== CIUDADES ===\n";
    for (const auto& c : cities) {
        std::cout <<  c.name << " (" << c.races.size() << " circuitos)\n";
        for (const auto& r : c.races)
            std::cout << "   - " << r.mapFile << "\n";
    }

    std::cout << "=============================\n\n";
}
