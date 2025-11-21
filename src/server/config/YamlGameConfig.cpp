#include "YamlGameConfig.h"

#include <iostream>
#include <ranges>
#include <utility>

YamlGameConfig::YamlGameConfig(const std::string& filePath) {
    try {
        root = YAML::LoadFile(filePath);
        std::cout << "[YamlGameConfig] Cargando configuración desde: "
                  << filePath << "\n";

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
                CarType type = getCarType(c["name"].as<std::string>());

                CarDisplayInfo display;
                display.name = c["name"].as<std::string>();
                display.description =
                    c["description"] ? c["description"].as<std::string>() : "";
                carsDisplayInfo[type] = std::move(display);

                CarStaticStats stats;
                stats.maxSpeed = c["max_speed"].as<float>();
                stats.acceleration = c["acceleration"].as<float>();
                stats.mass = c["mass"].as<float>();
                stats.control = c["control"].as<float>();
                stats.maxHealth = c["health"].as<float>();

                stats.nitroMultiplier = c["nitro_multiplier"].as<float>();
                stats.nitroDuration = c["nitro_duration"].as<float>();
                stats.nitroCooldown = c["nitro_cooldown"].as<float>();

                stats.width = c["width"].as<float>();
                stats.height = c["height"].as<float>();

                stats.density = c["density"] ? c["density"].as<float>() : 1.0f;
                stats.friction =
                    c["friction"] ? c["friction"].as<float>() : 0.8f;
                stats.restitution =
                    c["restitution"] ? c["restitution"].as<float>() : 0.1f;
                stats.linearDamping = c["linear_damping"]
                                          ? c["linear_damping"].as<float>()
                                          : 0.3f;
                stats.angularDamping = c["angular_damping"]
                                           ? c["angular_damping"].as<float>()
                                           : 0.8f;
                carsStaticStats[type] = std::move(stats);
            }
        } else {
            throw std::runtime_error(
                "Falta la sección 'car_types' en config.yaml");
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
    } catch (const std::exception& e) {
        std::cerr << "Error cargando config YAML: " << e.what() << std::endl;
        throw;
    }
}

const std::vector<RaceDefinition>& YamlGameConfig::getRaces(
    const CityId& city) const {
    for (const auto& c : cities) {
        if (c.name == city) return c.races;
    }
    throw std::invalid_argument("City not found: " + city);
}

CarType YamlGameConfig::getCarType(const std::string& name) {
    if (name == "Speedster") return CarType::Speedster;
    if (name == "Tank") return CarType::Tank;
    if (name == "Drifter") return CarType::Drifter;
    if (name == "Rocket") return CarType::Rocket;
    if (name == "Classic") return CarType::Classic;
    if (name == "Offroad") return CarType::Offroad;
    if (name == "Ghost") return CarType::Ghost;

    throw std::invalid_argument("Unknown car type: " + name);
}
void YamlGameConfig::printSummary() const {
    std::cout << "\n=== CONFIGURACIÓN GLOBAL ===\n";
    std::cout << "Jugadores máx: " << maxPlayers << "\n";
    std::cout << "Límite tiempo: " << timeLimitSec << " s\n";
    std::cout << "Intermission:   " << intermissionSec << " s\n";

    std::cout << "\n=== AUTOS DISPONIBLES ===\n";
    for (const auto& c : carsDisplayInfo | std::views::keys) {
        const auto& displayInfo = carsDisplayInfo.at(c);
        const auto& staticStats = carsStaticStats.at(c);
        std::cout << displayInfo.name << " (" << displayInfo.description << ")"
                  << std::endl
                  << "\tspeed=" << staticStats.maxSpeed
                  << ", acc=" << staticStats.acceleration
                  << ", mass=" << staticStats.mass
                  << ", control=" << staticStats.control
                  << ", hp=" << staticStats.maxHealth << ", nitro×"
                  << staticStats.nitroMultiplier << std::endl;
    }

    std::cout << "\n=== CIUDADES ===\n";
    for (const auto& c : cities) {
        std::cout << c.name << " (" << c.races.size() << " circuitos)\n";
        for (const auto& r : c.races) std::cout << "   - " << r.mapFile << "\n";
    }

    std::cout << "=============================\n\n";
}
