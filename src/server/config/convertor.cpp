#include "server/config/convertor.h"

Convertor::Convertor(const float pixelsPerMeter)
    : pixelsPerMeter(pixelsPerMeter) {}

float Convertor::toPixels(float meters) const {
    return meters * pixelsPerMeter;
}
float Convertor::toMeters(float pixels) const {
    return pixels / pixelsPerMeter;
}

Point Convertor::toPixels(Point point) const {
    return {toPixels(point.x), toPixels(point.y)};
}
Point Convertor::toMeters(Point point) const {
    return {toMeters(point.x), toMeters(point.y)};
}

Bound Convertor::toPixels(Bound bound) const {
    return {toPixels(bound.pos), toPixels(bound.width), toPixels(bound.height)};
}
Bound Convertor::toMeters(Bound bound) const {
    return {toMeters(bound.pos), toMeters(bound.width), toMeters(bound.height)};
}

float Convertor::toRadians(float degrees) {
    return degrees * std::numbers::pi / 180.0f;
}
float Convertor::toDegrees(float radians) {
    return radians * 180.0f / std::numbers::pi;
}
