#pragma once

#include "common/structs.h"

class Convertor {
    const float pixelsPerMeter;

   public:
    explicit Convertor(float pixelsPerMeter);

    float toPixels(float) const;
    float toMeters(float) const;

    Point toPixels(Point point) const;
    Point toMeters(Point point) const;
    Bound toPixels(Bound bound) const;
    Bound toMeters(Bound bound) const;

    static float toDegrees(float);
    static float toRadians(float);
};
