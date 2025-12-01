#include <SDL2/SDL.h>

#include <SDL2pp/SDL2pp.hh>
#include <iomanip>
#include <sstream>

#include "client/game/classes.h"
#include "common/structs.h"

Screen::Screen(SDL2pp::Renderer& renderer, Game& game,
               const CityName& city_name)
    : renderer(renderer),
      game(game),
      assets(renderer, city_name),
      WIDTH(renderer.GetOutputWidth()),
      HEIGHT(renderer.GetOutputHeight()) {
    renderer.Clear();
    renderer.SetDrawBlendMode(SDL_BLENDMODE_BLEND);
}

//
// METODOS DE RENDERIZADO BASE
//
void Screen::render(SDL2pp::Texture& texture, SDL2pp::Point pos, double angle,
                    bool in_world) {
    if (in_world) pos -= SDL2pp::Point(cam_offset_x, cam_offset_y);

    renderer.Copy(texture, SDL2pp::NullOpt, pos, angle);
}

void Screen::render_slice(SDL2pp::Texture& texture, SDL2pp::Rect section,
                          SDL2pp::Point pos, bool in_world) {
    if (in_world) pos -= SDL2pp::Point(cam_offset_x, cam_offset_y);

    renderer.Copy(texture, section, pos);
}

void Screen::render_text(const std::string& texto, SDL2pp::Point pos,
                         bool in_world) {
    SDL2pp::Surface s =
        assets.font.RenderText_Solid(texto, SDL_Color{255, 255, 255, 255});
    SDL2pp::Texture t(renderer, s);

    render(t, pos, 0, in_world);
}

void Screen::render_solid(SDL2pp::Rect rect, const SDL2pp::Color& color,
                          bool in_world) {
    if (in_world) rect -= SDL2pp::Point(cam_offset_x, cam_offset_y);

    renderer.SetDrawColor(color).FillRect(rect);
}

void Screen::render_solid(SDL2pp::Rect rect, const SDL2pp::Color& color,
                          double angle, bool in_world) {
    if (in_world) rect -= SDL2pp::Point(cam_offset_x, cam_offset_y);

    // Ya que SDL2 no soporta rectangulos rotados, usar una textura blanca
    // generica
    auto& white = assets.white;
    white.SetColorAndAlphaMod(color);
    renderer.Copy(white, SDL2pp::NullOpt, rect, angle);
}

//
// METODOS DE RENDERIZADO COMPUESTOS
//
void Screen::render_car(NpcCar& car) {
    SDL2pp::Texture& sprite = *assets.car_name.at(car.type);
    render(sprite, car.pos.get_top_left(), car.pos.get_angle());
}

void Screen::render_car(PlayerCar& car, bool with_name) {
    SDL2pp::Texture& sprite = *assets.car_name.at(car.type);
    SDL2pp::Point pos = car.pos.get_top_left();

    render(sprite, pos, car.pos.get_angle());
    if (with_name)
        render_text(car.name, pos - SDL2pp::Point(0, sprite.GetHeight() + 10));
}

//
// MANEJO DE CAMARA (VISUAL)
//
void Screen::update_camera() {
    cam_offset_x = game.cam_x - WIDTH / 2;
    cam_offset_y = game.cam_y - HEIGHT / 2;
}

//
// PASOS DE RENDERIZADO
//
void Screen::draw_ciudad() { render(*assets.city, {0, 0}); }

void Screen::draw_next_checkpoint() {
    if (!my_car) return;

    auto next_checkpoint = my_car->next_checkpoint;
    if (next_checkpoint < game.checkpoint_amount) {
        const BoundingBox& b = game.checkpoints[next_checkpoint];

        render_solid(b.as_rect(), {0, 255, 0, 150}, b.get_angle());
    }
}

void Screen::draw_coches(RenderLayer capa) {
    for (auto& [id, car] : game.cars) {
        if (car.layer != capa) continue;
        render_car(car, id != game.my_id);
    }

    for (auto& npc : game.npcs) {
        if (npc.layer != capa) continue;
        render_car(npc);
    }
}

void Screen::draw_bridges() {
    for (const BoundingBox& b : game.bridges)
        render_slice(*assets.city, b.as_rect(), b.get_top_left());
}

void Screen::draw_overpasses() {
    for (const BoundingBox& b : game.overpasses)
        render_slice(*assets.city, b.as_rect(), b.get_top_left());
}

static std::string format_time(float time) {
    std::ostringstream ss;
    ss << std::fixed << std::setprecision(0);

    int time_i = static_cast<int>(time);
    ss << time_i / 60 << ":";
    ss << std::setw(2) << std::setfill('0') << time_i % 60 << ".";
    ss << std::setw(2) << std::setfill('0') << fmod(time, 1) * 100;
    return ss.str();
}

void Screen::draw_hud() {
    if (!my_car) return;

    render_solid({10, 10, my_car->health, 10}, {0, 255, 0, 255}, false);
    render_solid({10, 30, my_car->speed, 10}, {255, 165, 0, 255}, false);

    auto next_checkpoint = my_car->next_checkpoint;
    if (next_checkpoint < game.checkpoint_amount) {
        const BoundingBox& b = game.checkpoints[next_checkpoint];

        float angle = my_car->pos.angle_to(b);
        render(assets.arrow, {WIDTH / 2 - assets.arrow.GetWidth() / 2, 10},
               angle, false);
    }

    if (my_car->finished) {
        render_solid({0, 0, WIDTH, HEIGHT}, {0, 0, 0, 200}, false);
        // TODO(franco): pantalla de resultado
    } else {
        render_text(format_time(game.time_elapsed), {10, 50}, false);
    }
}

void Screen::update() {
    my_car = game.my_car;
    renderer.Clear();
    update_camera();

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
