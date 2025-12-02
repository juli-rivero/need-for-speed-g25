#include "client/qt/windows/car_assets.h"

#include <unordered_map>

QString getCarImagePath(const CarType carType) {
    static const std::unordered_map<CarType, QString> carImageMap = {
        {CarType::Speedster, "assets/cars/speedster"},
        {CarType::Tank, "assets/cars/tank"},
        {CarType::Drifter, "assets/cars/drifter"},
        {CarType::Rocket, "assets/cars/rocket"},
        {CarType::Classic, "assets/cars/classic"},
        {CarType::Offroad, "assets/cars/offroad"},
        {CarType::Ghost, "assets/cars/ghost"}};

    if (not carImageMap.contains(carType)) return QString();
    return carImageMap.at(carType);
}
