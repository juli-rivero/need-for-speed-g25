
#ifndef TALLER_TP_CARTYPE_H
#define TALLER_TP_CARTYPE_H
#include <string>

// === Estructura de un tipo de auto ===
struct CarType {
    std::string name;
    std::string description;

    // --- atributos de rendimiento ---
    float maxSpeed;
    float acceleration;
    float mass;
    float control;
    mutable float health;

    // --- nitro ---
    float nitroMultiplier;
    float nitroDuration;
    float nitroCooldown;

    // --- físicas ---
    float width;
    float height;
    float density;
    float friction;
    float restitution;
    float linearDamping;
    float angularDamping;
};
#endif //TALLER_TP_CARTYPE_H