#include "CarSprite.h"
std::string CarSprite::getSprite(const CarType car) {
    switch (car) {
        case CarType::Classic:
            return "🚗";
        case CarType::Drifter:
            return "💨";
        case CarType::Ghost:
            return "👻";
        case CarType::Offroad:
            return "🚜";
        case CarType::Rocket:
            return "🚀";
        case CarType::Speedster:
            return "🏎️";
        case CarType::Tank:
            return "🛡️";
    }
    return "";
}
