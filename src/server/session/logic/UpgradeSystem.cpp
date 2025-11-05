
#include "UpgradeSystem.h"

std::unordered_map<PlayerId, float>
UpgradeSystem::applyForNextRace(
    const std::unordered_map<PlayerId, std::vector<UpgradeChoice>>& upsByPlayer) {

    std::unordered_map<PlayerId, float> penalties;

    for (auto& [pid, ups] : upsByPlayer) {
        float totalPenalty = 0.0f;
        for (const auto& up : ups) {
            // si no trae penalty ya calculada, consultá al config por stat
            float p = up.penalty;
            if (p == 0.0f) {
                // if (auto fromCfg = _cfg.penaltyFor(up.stat)) {
                //     p = *fromCfg;
                // }
            }
            totalPenalty += p;
            // TODO: aplicar el stat al auto en el armado de la siguiente carrera (no aquí)
        }
        penalties[pid] = totalPenalty;
    }
    return penalties;
}
