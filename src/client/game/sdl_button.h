#pragma once

#include <SDL2pp/SDL2pp.hh>
#include <functional>
#include <string>

class SdlButton {
    SDL2pp::Renderer& renderer;
    SDL2pp::Font& font;

    SDL2pp::Rect rect;  // x, y, w, h (coordenadas de pantalla)
    std::string label;

    SDL2pp::Color bg_color{50, 50, 50, 200};
    SDL2pp::Color bg_hover_color{70, 70, 70, 220};
    SDL2pp::Color border_color{255, 255, 255, 255};
    SDL2pp::Color text_color{255, 255, 255, 255};

    std::function<void()> on_click;

    bool hovered = false;

   public:
    SdlButton(SDL2pp::Renderer& renderer, SDL2pp::Font& font,
              const SDL2pp::Rect& rect, std::string label,
              std::function<void()> on_click = nullptr);

    void render();

    bool handle_event(const SDL_Event& e);
};
