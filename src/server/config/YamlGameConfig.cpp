#include "YamlGameConfig.h"
#include <iostream>

YamlGameConfig::YamlGameConfig(const std::string& filePath) {
    try {
        root = YAML::LoadFile(filePath);

        // precargar penalizaciones, si existen
        if (root["penalties"]) {
            for (auto it : root["penalties"]) {
                penalties[it.first.as<std::string>()] = it.second.as<float>();
            }
        }
    } catch (const std::exception& e) {
        std::cerr << " Error cargando config YAML: " << e.what() << std::endl;
        throw;
    }
}

int YamlGameConfig::maxPlayers() const {
    return root["race"]["max_players"].as<int>(8);
}

float YamlGameConfig::raceTimeLimitSec() const {
    return root["race"]["time_limit_sec"].as<float>(600.0f);
}

float YamlGameConfig::intermissionSec() const {
    return root["race"]["intermission_sec"].as<float>(10.0f);
}

std::optional<float> YamlGameConfig::penaltyFor(const std::string& statName) const {
    auto it = penalties.find(statName);
    if (it != penalties.end())
        return it->second;
    return std::nullopt;
}

float YamlGameConfig::defaultCarHealth() const {
    return root["car_defaults"]["health"].as<float>(100.0f);
}

float YamlGameConfig::defaultCarMaxSpeed() const {
    return root["car_defaults"]["max_speed"].as<float>(150.0f);
}

float YamlGameConfig::defaultCarAcceleration() const {
    return root["car_defaults"]["acceleration"].as<float>(10.0f);
}

float YamlGameConfig::defaultCarMass() const {
    return root["car_defaults"]["mass"].as<float>(1.0f);
}

float YamlGameConfig::defaultCarControl() const {
    return root["car_defaults"]["control"].as<float>(1.0f);
}