#include "MatchSession.h"

const char* MatchSession::toPenaltyKey(UpgradeStat s) {
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

void MatchSession::applyUpgrade(PlayerId id, UpgradeStat stat) {
    auto& player = _players.at(id);

    const auto& upgrades = _cfg.getUpgrades();
    const auto& penalties = _cfg.getPenalties();

    const char* key = toPenaltyKey(stat);

    float statIncrement = upgrades.at(key);
    float penaltyAdd = penalties.at(key);

    UpgradeChoice up{stat};
    player->getCar()->upgrade(up, statIncrement);

    float newPenalty = player->getPenalty() + penaltyAdd;
    player->setPenalty(newPenalty);
}
