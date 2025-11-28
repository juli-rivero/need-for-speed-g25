#include <SDL2/SDL.h>

#include <SDL2pp/SDL2pp.hh>
#include <iomanip>
#include <sstream>

#include "client/game/classes.h"
#include "common/structs.h"

Screen::Screen(SDL2pp::Renderer& renderer, Game& game)
    : renderer(renderer),
      game(game),
      WIDTH(renderer.GetOutputWidth()),
      HEIGHT(renderer.GetOutputHeight()) {
    renderer.Clear();
    renderer.SetDrawBlendMode(SDL_BLENDMODE_BLEND);
}

//
// METODOS DE RENDERIZADO
//
void Screen::render(SDL2pp::Texture& texture, SDL2pp::Point pos, double angle,
                    bool in_world) {
    if (in_world) pos -= SDL2pp::Point(game.cam_x, game.cam_y);

    renderer.Copy(texture, SDL2pp::NullOpt, pos, angle);
}

void Screen::render_slice(SDL2pp::Texture& texture, SDL2pp::Rect section,
                          SDL2pp::Point pos, bool in_world) {
    if (in_world) pos -= SDL2pp::Point(game.cam_x, game.cam_y);

    renderer.Copy(texture, section, pos);
}

void Screen::render_text(const std::string& texto, SDL2pp::Point pos,
                         bool in_world) {
    SDL2pp::Surface s =
        game.assets.font.RenderText_Solid(texto, SDL_Color{255, 255, 255, 255});
    SDL2pp::Texture t(renderer, s);

    render(t, pos, 0, in_world);
}

void Screen::render_solid(SDL2pp::Rect rect, const SDL2pp::Color& color,
                          bool in_world) {
    if (in_world) rect -= SDL2pp::Point(game.cam_x, game.cam_y);

    renderer.SetDrawColor(color).FillRect(rect);
}

void Screen::render_solid(SDL2pp::Rect rect, const SDL2pp::Color& color,
                          double angle, bool in_world) {
    if (in_world) rect -= SDL2pp::Point(game.cam_x, game.cam_y);

    // FIXME(franco): hack.
    auto& white = game.assets.white;
    white.SetColorAndAlphaMod(color);
    renderer.Copy(white, SDL2pp::NullOpt, rect, angle);
}

//
// PASOS DE RENDERIZADO
//
void Screen::draw_ciudad() {
    auto& city = *game.assets.city_name.at(game.city_info.name);
    render(city, {0, 0});
}

void Screen::draw_next_checkpoint() {
    if (!my_car) return;

    auto next_checkpoint = my_car->next_checkpoint;

    if (next_checkpoint < game.map.checkpoint_amount) {
        const Map::Box& b = game.map.checkpoints[next_checkpoint];

        // FIXME(franco): hack para dibujar rectangulo verde rotado...
        render_solid({b.x, b.y, b.w, b.h}, {0, 255, 0, 150}, b.angle);
    }
}

void Screen::draw_coches(RenderLayer capa) {
    for (auto& [id, car] : game.cars) {
        if (car.layer != capa) continue;
        car.draw(id != game.my_id);
    }
}

void Screen::draw_bridges() {
    auto& city = *game.assets.city_name.at(game.city_info.name);

    for (const Map::Box& b : game.map.bridges)
        render_slice(city, {b.x, b.y, b.w, b.h}, {0, 0});
}

void Screen::draw_overpasses() {
    auto& city = *game.assets.city_name.at(game.city_info.name);

    for (const Map::Box& b : game.map.overpasses)
        render_slice(city, {b.x, b.y, b.w, b.h}, {0, 0});
}

static std::string format_time(float time) {
    std::ostringstream ss;
    ss << std::fixed << std::setprecision(0);

    // Si, esto definitivamente es mejor que una llamada a sprintf.
    // Definitivamente.
    int time_i = static_cast<int>(time);
    ss << time_i / 60 << ":";
    ss << std::setw(2) << std::setfill('0') << time_i % 60 << ".";
    ss << std::setw(2) << std::setfill('0') << fmod(time, 1) * 100;
    return ss.str();
}

void Screen::draw_hud() {
    if (!my_car) return;

    render_solid({10, 10, static_cast<int>(my_car->health), 10},
                 {0, 255, 0, 255}, false);
    render_solid({10, 30, static_cast<int>(my_car->speed), 10},
                 {255, 165, 0, 255}, false);

    bool has_angle = false;
    float angle = my_car->get_angle_to_next_checkpoint(has_angle);
    if (has_angle) {
        render(game.assets.arrow,
               {WIDTH / 2 - game.assets.arrow.GetWidth() / 2, 10}, angle,
               false);
    }

    if (my_car->finished) {
        render_solid({0, 0, WIDTH, HEIGHT}, {0, 0, 0, 200}, false);
        // TODO(franco): pantalla de resultado
    } else {
        render_text(format_time(my_car->elapsed_time), {10, 50}, false);
    }
}

void Screen::update() {
    my_car = game.my_car;
    if (my_car) my_car->set_camera();

    renderer.Clear();

    // Capa 1: mapa y sus detalles
    draw_ciudad();
    draw_next_checkpoint();

    // Capa 2: coches
    draw_coches(RenderLayer::UNDER);
    draw_bridges();
    draw_coches(RenderLayer::OVER);
    draw_overpasses();

    // Capa 3: interfaz
    draw_hud();

    renderer.SetDrawColor(0, 0, 0, 255);
    renderer.Present();
}
