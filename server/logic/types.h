
#ifndef TALLER_TP_TYPES_H
#define TALLER_TP_TYPES_H
#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

using PlayerId = std::uint32_t;

enum class CityId { LibertyCity, SanAndreas, ViceCity };

struct PlayerResult {
    PlayerId id{};
    float rawTime{0.0f};     // tiempo crudo al llegar a meta
    float penalty{0.0f};     // penalización aplicada en esta carrera
    float netTime{0.0f};     // rawTime - penalty
    bool dnf{false};         // no terminó / descalificado
};

struct RaceDefinition {
    CityId city;
    std::string mapFile;     // YAML del recorrido (checkpoints/hints)
};

struct UpgradeChoice {
    // ejemplo: "max_speed", "acceleration"
    std::string stat;
    float delta{0.0f};
    float penalty{0.0f};     // segundos que se aplicarán en la próxima carrera
};
#endif //TALLER_TP_TYPES_H