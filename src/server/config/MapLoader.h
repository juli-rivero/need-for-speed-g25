#pragma once

#include <string>
#include <vector>

#include "common/structs.h"
#include "server/config/YamlGameConfig.h"
#include "server/config/convertor.h"
#include "server/session/NPC/NPCType.h"
#include "yaml-cpp/yaml.h"

class MapLoader {
   public:
    const YAML::Node root;
    const Convertor convertor;

    explicit MapLoader(const std::string& file, const YamlGameConfig& cfg);

    std::string getCityName() const;
    std::string getCircuitName() const;

    std::vector<Bound> getWalls() const;
    std::vector<Bound> getRailings() const;
    std::vector<Bound> getBridges() const;
    std::vector<Bound> getOverpasses() const;
    std::vector<SpawnPointInfo> getSpawnPoints() const;
    std::vector<CheckpointInfo> getCheckpoints() const;
    std::vector<Bound> getUpperSensors() const;
    std::vector<Bound> getLowerSensors() const;
    std::vector<RoadShape> getRoadShapes() const;

   private:
    Bound parseBound(const YAML::Node& node) const;
};
