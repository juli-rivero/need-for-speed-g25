
#ifndef TALLER_TP_UPGRADESYSTEM_H
#define TALLER_TP_UPGRADESYSTEM_H

#include <unordered_map>
#include <vector>
#include "types.h"
#include "IGameConfig.h"

class UpgradeSystem {
public:
    explicit UpgradeSystem(const IGameConfig& cfg) : _cfg(cfg) {}

    // inserta mejoras y calcula penalizaciones para la PRÓXIMA carrera
    std::unordered_map<PlayerId, float>
    applyForNextRace(const std::unordered_map<PlayerId, std::vector<UpgradeChoice>>& upsByPlayer);

private:
    const IGameConfig& _cfg;
};

#endif //TALLER_TP_UPGRADESYSTEM_H