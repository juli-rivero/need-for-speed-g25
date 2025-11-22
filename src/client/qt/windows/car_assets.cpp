#include "client/qt/windows/car_assets.h"

#include <unordered_map>

QString getCarImagePath(CarSpriteType carType) {
    static const std::unordered_map<CarSpriteType, int> carImageMap = {
        {CarSpriteType::Speedster, 1}, {CarSpriteType::Tank, 2},
        {CarSpriteType::Drifter, 3},   {CarSpriteType::Rocket, 4},
        {CarSpriteType::Classic, 5},   {CarSpriteType::Offroad, 6},
        {CarSpriteType::Ghost, 7}};

    int imageNumber = 1;  // default

    auto it = carImageMap.find(carType);
    if (it != carImageMap.end()) {
        imageNumber = it->second;
    }

    return QString("assets/cars/car%1.png").arg(imageNumber);
}
