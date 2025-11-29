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

    // Configuración global de carrera
    int maxRaces{};
    int maxPlayers{};
    int maxNPCs{};
    float timeLimitSec{};
    float intermissionSec{};

   public:
    explicit YamlGameConfig(const std::string& filePath);

    // Getters
    int getMaxRacesPerSession() const { return maxRaces; }
    int getMaxPlayers() const { return maxPlayers; }
    float getRaceTimeLimitSec() const { return timeLimitSec; }
    float getIntermissionSec() const { return intermissionSec; }
    int getMaxNPCS() const { return maxNPCs; }

    const std::unordered_map<std::string, float>& getPenalties() const {
        return penalties;
    }

    const std::vector<CityName> getAvailableCities() const;
    const std::vector<RaceName>& getRaces(const CityName& city) const;
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
