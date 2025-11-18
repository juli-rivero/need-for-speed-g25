#include "CarSprite.h"
std::string CarSprite::getSprite(const CarSpriteType car) {
    switch (car) {
        case CarSpriteType::Classic:
            return "🚗";
        case CarSpriteType::Drifter:
            return "💨";
        case CarSpriteType::Ghost:
            return "👻";
        case CarSpriteType::Offroad:
            return "🚜";
        case CarSpriteType::Rocket:
            return "🚀";
        case CarSpriteType::Speedster:
            return "🏎️";
        case CarSpriteType::Tank:
            return "🛡️";
    }
    return "";
}
