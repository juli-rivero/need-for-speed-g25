#include "client/game/bounding_box.h"

BoundingBox::BoundingBox(const Bound& b, float angle) {
    // Extraer datos relevantes
    x_float = b.pos.x;
    y_float = b.pos.y;
    w_float = b.width;
    h_float = b.height;

    // (x, y) apunta al centro, no a la esquina.
    x_float -= w_float / 2;
    y_float -= h_float / 2;

    // Ahora si, esta listo para el resto del juego.
    // Conseguir la version en int, para ahorrar el costo de casteos.
    x_int = static_cast<int>(x_float);
    y_int = static_cast<int>(y_float);
    w_int = static_cast<int>(w_float);
    h_int = static_cast<int>(h_float);

    this->angle = angle;
}

float BoundingBox::get_x() const { return x_float; }
float BoundingBox::get_y() const { return y_float; }
float BoundingBox::get_w() const { return w_float; }
float BoundingBox::get_h() const { return h_float; }
float BoundingBox::get_angle() const { return angle; }

SDL2pp::Rect BoundingBox::as_rect() const {
    return SDL2pp::Rect(x_int, y_int, w_int, h_int);
}

SDL2pp::Point BoundingBox::get_top_left() const {
    return SDL2pp::Point(x_int, y_int);
}

SDL2pp::Point BoundingBox::get_center() const {
    return SDL2pp::Point(x_int + w_int / 2, y_int + h_int / 2);
}

float BoundingBox::angle_to(const BoundingBox& other) const {
    // Conseguir las posiciones al centro
    float x_this = this->x_float + this->w_float / 2;
    float y_this = this->y_float + this->h_float / 2;

    float x_other = other.get_x() + other.get_w() / 2;
    float y_other = other.get_y() + other.get_h() / 2;

    // Calcular el angulo
    return atan2f(y_this - y_other, x_this - x_other) * 180 / std::numbers::pi;
}

float BoundingBox::distance_to(int x, int y) const {
    double dx = x - this->x_float;
    double dy = y - this->y_float;

    return sqrt(dx * dx + dy * dy);
}
