#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "../session/logic/types.h"
#include "../session/model/CarType.h"
#include "common/structs.h"
#include "yaml-cpp/yaml.h"

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

    const std::unordered_map<std::string, float>& getPenalties() const {
        return penalties;
    }
    const std::vector<CityDefinition>& getCities() const { return cities; }
    const std::vector<CarType>& getCarTypes() const { return carTypes; }
    static CarSpriteType getCarSpriteType(const std::string& name);

    void printSummary() const;
};
