#pragma once

#include <memory>
#include <string>
#include <vector>

#include "common/structs.h"

using PlayerId = std::uint32_t;

using CityId = std::string;

struct SpawnPoint {
    float x, y, angle;
};
struct PlayerConfig {
    PlayerId id;              // id único del jugador
    std::string name;         // nombre visible
    std::string carTypeName;  // nombre del tipo de auto (Speedster, Tank, etc.)
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
struct UpgradeChoice {
    // ejemplo: "max_speed", "acceleration"
    std::string stat;
    float delta{0.0f};
    float penalty{0.0f};  // segundos que se aplicarán en la próxima carrera
};
