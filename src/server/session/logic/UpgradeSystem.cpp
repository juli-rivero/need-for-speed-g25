
#include "UpgradeSystem.h"

#include <iostream>

const char* UpgradeSystem::toPenaltyKey(UpgradeStat s) {
    switch (s) {
        case UpgradeStat::MaxSpeed:
            return "max_speed";
        case UpgradeStat::Acceleration:
            return "acceleration";
        case UpgradeStat::Health:
            return "health";
        case UpgradeStat::Nitro:
            return "nitro";  // si querés, agregás esto en el YAML
        default:
            return nullptr;
    }
}

std::unordered_map<PlayerId, float> UpgradeSystem::applyForNextRace(
    const std::unordered_map<PlayerId, std::vector<UpgradeChoice>>& ups) {
    std::unordered_map<PlayerId, float> result;

    const auto& penTable = _cfg.getPenalties();

    for (const auto& [playerId, choices] : ups) {
        float totalPenalty = 0.0f;

        for (const auto& up : choices) {
            const char* key = toPenaltyKey(up.stat);
            if (!key) continue;

            auto it = penTable.find(key);
            if (it == penTable.end()) {
                continue;
            }

            const float basePenalty = it->second;
            totalPenalty += basePenalty * up.delta;
        }

        result[playerId] = totalPenalty;
    }

    return result;
}
