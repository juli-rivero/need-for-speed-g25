
#ifndef TALLER_TP_YAMLGAMECONFIG_H
#define TALLER_TP_YAMLGAMECONFIG_H

#include "yaml-cpp/yaml.h"
#include <unordered_map>
#include <string>

#include "../session/model/CarType.h"
#include "../session/logic/types.h"

struct CityDefinition;

class YamlGameConfig {
private:
    YAML::Node root;

    std::unordered_map<std::string, float> penalties;
    std::vector<CityDefinition> cities;
    std::vector<CarType> carTypes;

    // Configuración global de carrera
    int maxPlayers{};
    float timeLimitSec{};
    float intermissionSec{};

public:
    explicit YamlGameConfig(const std::string& filePath);

    // Getters
    int getMaxPlayers() const { return maxPlayers; }
    float getRaceTimeLimitSec() const { return timeLimitSec; }
    float getIntermissionSec() const { return intermissionSec; }

    const std::unordered_map<std::string, float>& getPenalties() const { return penalties; }
    const std::vector<CityDefinition>& getCities() const { return cities; }
    const std::vector<CarType>& getCarTypes() const { return carTypes; }

    void printSummary() const;
};
#endif
