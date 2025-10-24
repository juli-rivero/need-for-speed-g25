//
// Created by elvis on 23/10/25.
//

#ifndef TALLER_TP_IGAMECONFIG_H
#define TALLER_TP_IGAMECONFIG_H

#include <string>
#include <optional>

class IGameConfig {
public:
    virtual ~IGameConfig() = default;

    // carrera/partida
    virtual int   maxPlayers() const = 0;
    virtual float raceTimeLimitSec() const = 0;
    virtual float intermissionSec() const = 0;

    // penalizaciones por mejoras
    virtual std::optional<float> penaltyFor(const std::string& statName) const = 0;

    // valores por defecto del auto
    virtual float defaultCarHealth() const = 0;
    virtual float defaultCarMaxSpeed() const = 0;
    virtual float defaultCarAcceleration() const = 0;
    virtual float defaultCarMass() const = 0;
    virtual float defaultCarControl() const = 0;
};

#endif //TALLER_TP_IGAMECONFIG_H