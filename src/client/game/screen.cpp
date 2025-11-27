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
void Screen::render(SDL2pp::Texture& texture, int x, int y, double angle,
                    bool in_world) {
    SDL2pp::Point pos(x, y);
    if (in_world) pos -= SDL2pp::Point(game.cam_x, game.cam_y);

    renderer.Copy(texture, SDL2pp::NullOpt, pos, angle);
}

void Screen::render(const std::string& texto, int x, int y, bool in_world) {
    SDL2pp::Surface s =
        game.assets.font.RenderText_Solid(texto, SDL_Color{255, 255, 255, 255});
    SDL2pp::Texture t(renderer, s);

    render(t, x, y, 0, in_world);
}

void Screen::render_rect(SDL2pp::Rect rect, const SDL2pp::Color& color,
                         bool in_world) {
    if (in_world) rect -= SDL2pp::Point(game.cam_x, game.cam_y);

    renderer.SetDrawColor(color).FillRect(rect);
}

//
// PASOS DE RENDERIZADO
//
void Screen::draw_ciudad() {
    auto& city = *game.assets.city_name.at(game.city_info.name);
    render(city, 0, 0);
}

void Screen::draw_checkpoint() {
    const auto& checkpoints = game.race_info.checkpoints;

    if (my_car && my_car->next_checkpoint < checkpoints.size()) {
        const CheckpointInfo& c = checkpoints[my_car->next_checkpoint];
        const Bound& cb = c.bound;

        render_rect(
            {static_cast<int>(cb.pos.x * 10), static_cast<int>(cb.pos.y * 10),
             static_cast<int>(cb.width * 10), static_cast<int>(cb.height * 10)},
            {0, 255, 0, 128});
    }
}

void Screen::draw_coches() {
    for (auto& [id, car] : game.cars) {
        if (id == game.my_id) continue;
        car.draw(true);
    }
    if (my_car) my_car->draw(false);
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

    render_rect({10, 10, static_cast<int>(my_car->health), 10},
                {0, 255, 0, 255}, false);
    render_rect({10, 30, static_cast<int>(my_car->speed), 10},
                {255, 165, 0, 255}, false);

    bool has_angle = false;
    float angle = my_car->get_angle_to_next_checkpoint(has_angle);
    if (has_angle) {
        render(game.assets.arrow, WIDTH / 2 - game.assets.arrow.GetWidth() / 2,
               10, angle, false);
    }

    if (my_car->finished) {
        render_rect({0, 0, WIDTH, HEIGHT}, {0, 0, 0, 200}, false);
        // TODO(franco): pantalla de resultado
    } else {
        render(format_time(my_car->elapsed_time), 10, 50, false);
    }
}

void Screen::update() {
    my_car = game.my_car;
    if (my_car) my_car->set_camera();

    renderer.Clear();

    draw_ciudad();
    draw_checkpoint();
    draw_coches();
    draw_hud();

    renderer.SetDrawColor(0, 0, 0, 255);
    renderer.Present();
}
