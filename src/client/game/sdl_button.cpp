#include "client/game/sdl_button.h"

#include <SDL2/SDL.h>

#include <utility>

SdlButton::SdlButton(SDL2pp::Renderer& renderer, SDL2pp::Font& font,
                     const SDL2pp::Rect& rect, std::string label,
                     std::function<void()> on_click)
    : renderer(renderer),
      font(font),
      rect(rect),
      label(std::move(label)),
      on_click(std::move(on_click)) {}

void SdlButton::render() {
    // Fondo (hover o normal)
    const SDL2pp::Color& bg = hovered ? bg_hover_color : bg_color;
    renderer.SetDrawColor(bg).FillRect(rect);

    // Borde
    renderer.SetDrawColor(border_color).DrawRect(rect);

    // Texto centrado
    SDL2pp::Surface s = font.RenderText_Solid(label, text_color);
    SDL2pp::Texture t(renderer, s);

    int text_w = t.GetWidth();
    int text_h = t.GetHeight();
    int x = rect.GetX() + (rect.GetW() - text_w) / 2;
    int y = rect.GetY() + (rect.GetH() - text_h) / 2;
    renderer.Copy(t, SDL2pp::NullOpt, SDL2pp::Rect(x, y, text_w, text_h));
}

static bool point_in_rect(int x, int y, const SDL2pp::Rect& r) {
    return x >= r.GetX() && x < (r.GetX() + r.GetW()) && y >= r.GetY() &&
           y < (r.GetY() + r.GetH());
}

bool SdlButton::handle_event(const SDL_Event& e) {
    switch (e.type) {
        case SDL_MOUSEMOTION: {
            int mx = e.motion.x;
            int my = e.motion.y;
            bool mouse_in_rect = point_in_rect(mx, my, rect);
            if (mouse_in_rect != hovered) hovered = mouse_in_rect;
            return mouse_in_rect;  // se considera consumido si esta arriba
        }
        case SDL_MOUSEBUTTONUP: {
            if (e.button.button == SDL_BUTTON_LEFT) {
                int mx = e.button.x;
                int my = e.button.y;
                if (point_in_rect(mx, my, rect)) {
                    if (on_click) on_click();
                    return true;
                }
            }
            break;
        }
        default:
            break;
    }
    return false;
}
