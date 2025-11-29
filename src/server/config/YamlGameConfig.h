#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "common/structs.h"
#include "server/session/logic/types.h"
#include "yaml-cpp/yaml.h"

struct CityDefinition;

class YamlGameConfig {
   private:
    YAML::Node root;

    std::unordered_map<std::string, float> penalties;
    std::unordered_map<CityName, std::vector<std::string>> races;
    std::unordered_map<CarType, CarDisplayInfo> carsDisplayInfo;
    std::unordered_map<CarType, CarStaticStats> carsStaticStats;
    std::unordered_map<std::string, float> upgradesTable;
    std::unordered_map<std::string, float> penaltiesTable;

    // Configuración global de carrera
    int maxPlayers{};
    int maxNPCs{};
    float timeLimitSec{};
    float intermissionSec{};

   public:
    explicit YamlGameConfig(const std::string& filePath);

    // Getters
    int getMaxPlayers() const { return maxPlayers; }
    float getRaceTimeLimitSec() const { return timeLimitSec; }
    float getIntermissionSec() const { return intermissionSec; }
    int getMaxNPCS() const { return maxNPCs; }
    const auto& getUpgrades() const { return upgradesTable; }
    const auto& getPenalties() const { return penaltiesTable; }

    const std::vector<std::string>& getRaces(const CityName& city) const;
    const std::unordered_map<CarType, CarDisplayInfo>& getCarDisplayInfoMap()
        const {
        return carsDisplayInfo;
    }
    const std::unordered_map<CarType, CarStaticStats>& getCarStaticStatsMap()
        const {
        return carsStaticStats;
    }
    static constexpr auto DefaultCar = CarType::Classic;

    void printSummary() const;

   private:
    static CarType getCarType(const std::string& name);
};
