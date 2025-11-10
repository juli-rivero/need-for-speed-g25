#pragma once

#include <cstdint>
#include <string>

enum class SessionStatus { Waiting, Playing, Full };

#define SESSION_CONFIG_FIELDS \
    std::string name;         \
    uint8_t maxPlayers;       \
    uint8_t raceCount;        \
    std::string city;

// Estructura para crear una partida
struct SessionConfig {
    SESSION_CONFIG_FIELDS
};

// Estructura para representar una partida
struct SessionInfo {
    SESSION_CONFIG_FIELDS
    uint8_t currentPlayers;
    SessionStatus status;
};

// Estructura para representar un jugador en la sala de espera
struct PlayerInfo {
    int id;
    std::string name;
    int carType;
    bool isReady;
    bool isHost;
};

enum class CarSpriteType {
    Speedster,
    Tank,
    Drifter,
    Rocket,
    Classic,
    Offroad,
    Ghost
};

struct CarStaticInfo {
    CarSpriteType type;

    std::string name;
    std::string description;

    float height;
    float width;

    float maxSpeed;
    float acceleration;
    float mass;
    float control;
    mutable float health;
};
