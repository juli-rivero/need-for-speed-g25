#pragma once

#include <string>
#include <vector>

#include "common/structs.h"

struct PlayerInput {
    bool accelerate{false};
    bool brake{false};
    bool nitro{false};
    TurnDirection turn{TurnDirection::None};
};

struct WorldSnapshot {
    float time{0.0f};  // tiempo global simulado

    // estado de MatchSession
    MatchState matchState{MatchState::Starting};
    uint32_t currentRaceIndex{0};

    // estado de RaceSession
    RaceState raceState{RaceState::Countdown};
    float raceElapsed{0.0f};
    float raceCountdown{0.0f};

    float raceTimeLeft{0.0f};  // tiempo restante si hay límite (10min)
    std::vector<PlayerSnapshot> players;
};
