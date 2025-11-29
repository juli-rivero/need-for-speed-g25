#pragma once

#include <list>
#include <vector>

#include "common/macros.h"
#include "common/structs.h"

class Map final {
   public:
    struct Box {
        int x, y, w, h;
        float angle;
    };

    Map(const CityInfo& city_info, const RaceInfo& race_info);

    const CityName name;

    std::list<Box> bridges;
    std::list<Box> overpasses;

    std::vector<Box> checkpoints;
    size_t checkpoint_amount;

   private:
    Box convert_coords(const Bound& b, float angle = 0);
};
