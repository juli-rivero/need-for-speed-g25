#pragma once

#include <memory>
#include <string>
#include <vector>

#include "common/structs.h"

using CityId = std::string;

struct SpawnPoint {
    float x, y, angle;
};
struct PlayerConfig {
    PlayerId id;       // id único del jugador
    std::string name;  // nombre visible
    CarType carType;   // nombre del tipo de auto (Speedster, Tank, etc.)
};

struct PlayerResult {
    PlayerId id{};
    float rawTime{0.0f};  // tiempo crudo al llegar a meta
    float penalty{0.0f};  // penalización aplicada en esta carrera
    float netTime{0.0f};  // rawTime - penalty
    bool dnf{false};      // no terminó / descalificado
};

struct RaceDefinition {
    CityId city;
    std::string mapFile;  // YAML del recorrido (checkpoints/hints)
};
// Representa una ciudad con sus circuitos
struct CityDefinition {
    CityId name;
    std::vector<RaceDefinition> races;
};

enum class UpgradeStat { Acceleration, MaxSpeed, Nitro, Health };
struct UpgradeChoice {
    UpgradeStat stat;
    float delta;  // cuanto aumenta al stat, +1, +2 ,..
};
struct EndRaceUpgradeReport {
    PlayerId id;
    float penaltyTime;
    std::vector<UpgradeChoice> upgrades;
};

struct EndRaceSummaryPacket {
    uint32_t raceIndex;
    std::vector<EndRaceUpgradeReport> results;
};
