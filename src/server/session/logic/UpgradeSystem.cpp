
#include "UpgradeSystem.h"

std::unordered_map<PlayerId, float> UpgradeSystem::applyForNextRace(
    const std::unordered_map<PlayerId, std::vector<UpgradeChoice>>&
        upsByPlayer) {
    std::unordered_map<PlayerId, float> out;

    for (const auto& [pid, vec] : upsByPlayer) {
        float totalPenalty = 0.0f;

        for (const auto& up : vec) {
            totalPenalty += up.penalty;  // ← del YAML
        }

        out[pid] = totalPenalty;
    }

    return out;
}
