
#ifndef TALLER_TP_YAMLGAMECONFIG_H
#define TALLER_TP_YAMLGAMECONFIG_H
#include "../logic/IGameConfig.h"
#include "yaml-cpp/yaml.h"
#include <unordered_map>
#include <string>

class YamlGameConfig : public IGameConfig {
private:
    YAML::Node root;
    std::unordered_map<std::string, float> penalties;

public:
    explicit YamlGameConfig(const std::string& filePath);

    int   maxPlayers() const override;
    float raceTimeLimitSec() const override;
    float intermissionSec() const override;

    std::optional<float> penaltyFor(const std::string& statName) const override;

    float defaultCarHealth() const override;
    float defaultCarMaxSpeed() const override;
    float defaultCarAcceleration() const override;
    float defaultCarMass() const override;
    float defaultCarControl() const override;
};
#endif
