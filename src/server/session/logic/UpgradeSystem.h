
#pragma once

#include <unordered_map>
#include <vector>

#include "../../config/YamlGameConfig.h"
#include "server/session/logic/types.h"

class UpgradeSystem {
   public:
    explicit UpgradeSystem(const YamlGameConfig& cfg) : _cfg(cfg) {}

    // inserta mejoras y calcula penalizaciones para la PRÓXIMA carrera
    std::unordered_map<PlayerId, float> applyForNextRace(
        const std::unordered_map<PlayerId, std::vector<UpgradeChoice>>&
            upsByPlayer);

   private:
    const YamlGameConfig& _cfg;
};
