#include "client/game/map.h"

#include "spdlog/spdlog.h"

Map::Box Map::convert_coords(const Bound& b, float angle) {
    // Extraer datos relevantes
    float x = b.pos.x, y = b.pos.y, w = b.width, h = b.height;

    // Aplicar factor de scaling
    x *= 10;
    y *= 10;
    w *= 10;
    h *= 10;

    // (x, y) apunta al centro, no a la esquina.
    x -= w / 2;
    y -= h / 2;

    // Ahora si, esta listo para el resto del juego.
    return {static_cast<int>(x), static_cast<int>(y), static_cast<int>(w),
            static_cast<int>(h), angle};
}

Map::Map(const CityInfo& city_info, const RaceInfo& race_info)
    : name(city_info.name) {
    // Ajuste de datos de coordenadas...
    for (const Bound& b : city_info.bridges) {
        bridges.emplace_back(convert_coords(b));
    }
    for (const Bound& b : city_info.railings) {
        bridges.emplace_back(convert_coords(b));
    }

    for (const Bound& b : city_info.overpasses) {
        overpasses.emplace_back(convert_coords(b));
    }

    for (const CheckpointInfo& ci : race_info.checkpoints) {
        checkpoints.emplace_back(convert_coords(ci.bound, ci.angle));
        spdlog::info("{}", ci.angle);
    }
    checkpoint_amount = checkpoints.size();
}
