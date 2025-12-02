#pragma once

#include <string>
#include <vector>

#include "common/structs.h"

struct GameSetUp {
    std::vector<UpgradeChoice> upgrade_choices;
    CityInfo city_info;
    RaceInfo race_info;
};
