
#ifndef TALLER_TP_YAMLGAMECONFIG_H
#define TALLER_TP_YAMLGAMECONFIG_H

#include "yaml-cpp/yaml.h"
#include <unordered_map>
#include <string>

#include "../logic/types.h"

class YamlGameConfig {
private:
    YAML::Node root;
    std::unordered_map<std::string, float> penalties;
    std::vector<CityDefinition> cities;
    // Race
    int maxPlayers{8};
    float timeLimitSec{600.0f};
    float intermissionSec{10.0f};
    // Car defaults
    float defaultHealth{100.0f};
    float defaultMaxSpeed{150.0f};
    float defaultAcceleration{10.0f};
    float defaultMass{1.0f};
    float defaultControl{1.0f};
public:
    explicit YamlGameConfig(const std::string& filePath);
    // Getters
    int getMaxPlayers() const { return maxPlayers; }
    float getRaceTimeLimitSec() const { return timeLimitSec; }
    float getIntermissionSec() const { return intermissionSec; }

    float getDefaultHealth() const { return defaultHealth; }
    float getDefaultMaxSpeed() const { return defaultMaxSpeed; }
    float getDefaultAcceleration() const { return defaultAcceleration; }
    float getDefaultMass() const { return defaultMass; }
    float getDefaultControl() const { return defaultControl; }

    const std::unordered_map<std::string, float>& getPenalties() const { return penalties; }
    const std::vector<CityDefinition>& getCities() const { return cities; }

    void printSummary() const;


};
#endif
