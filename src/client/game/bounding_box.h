#pragma once

#include <SDL2pp/SDL2pp.hh>

#include "common/structs.h"

class BoundingBox final {
   private:
    int x_int, y_int, w_int, h_int;
    float x_float, y_float, w_float, h_float;

    float angle;

   public:
    explicit BoundingBox(const Bound& b, float angle = 0);

    float get_x() const;
    float get_y() const;
    float get_w() const;
    float get_h() const;
    float get_angle() const;

    SDL2pp::Point get_top_left() const;
    SDL2pp::Point get_center() const;
    SDL2pp::Rect as_rect() const;

    float angle_to(const BoundingBox& other) const;
    float distance_to(int x, int y) const;
    // Los bounding boxes se pueden copiar y mover.
};
