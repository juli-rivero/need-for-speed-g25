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

    minimap_texture = std::make_unique<SDL2pp::Texture>(
        renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, WIDTH,
        HEIGHT);
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
                         const SDL2pp::Color color, bool in_world) {
    SDL2pp::Surface s = assets.font.RenderText_Solid(texto, color);
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
    if (car.disqualified) return;
    // TODO(franco): explosion

    SDL2pp::Texture& sprite = *assets.car_name.at(car.type);
    SDL2pp::Point pos = car.pos.get_top_left();

    render(sprite, pos, car.pos.get_angle());
    if (with_name)
        render_text(car.name, pos - SDL2pp::Point(0, sprite.GetHeight() + 10),
                    {255, 255, 255, 255});
}

//
// MANEJO DE CAMARA (VISUAL)
//
void Screen::update_camera() {
    cam_offset_x = game.cam_x - this->WIDTH / 2;
    cam_offset_y = game.cam_y - this->HEIGHT / 2;
}

void Screen::update_camera(int width, int height) {
    cam_offset_x = game.cam_x - width / 2;
    cam_offset_y = game.cam_y - height / 2;
}

//
// PASOS DE RENDERIZADO
//
void Screen::draw_ciudad() { render(*assets.city, {0, 0}); }

void Screen::draw_next_checkpoint() {
    if (!game.my_car) return;

    auto next_checkpoint = game.my_car->next_checkpoint;
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
    render_solid({10, 10, game.my_car->health, 10}, {0, 255, 0, 255}, false);
    render_solid({10, 30, game.my_car->speed, 10}, {255, 165, 0, 255}, false);

    if (game.cheat_used) {
        render_text("TRAMPOSO", {10, 50}, {255, 255, 0, 255}, false);
    } else {
        render_text(format_time(game.time_elapsed), {10, 50},
                    {255, 255, 255, 255}, false);
    }

    auto next_checkpoint = game.my_car->next_checkpoint;
    if (next_checkpoint < game.checkpoint_amount) {
        const BoundingBox& b = game.checkpoints[next_checkpoint];

        float angle = game.my_car->pos.angle_to(b);
        render(assets.arrow, {WIDTH / 2 - assets.arrow.GetWidth() / 2, 10},
               angle, false);
    }
}

void Screen::draw_minimap() {
    // Temporalmente dibujar todo en el minimapa
    renderer.SetTarget(*minimap_texture);
    renderer.Clear();

    // Base y paredes
    render_solid({0, 0, WIDTH, HEIGHT}, {100, 100, 100, 255}, false);
    for (const BoundingBox& b : game.walls)
        render_solid(b.as_rect(), {255, 0, 0, 255});

    // Jugador
    const BoundingBox& my_b = game.my_car->pos;
    render_solid(my_b.as_rect(), {255, 255, 0, 255}, my_b.get_angle() + 90);

    // Checkpoint
    auto next_checkpoint = game.my_car->next_checkpoint;
    if (next_checkpoint < game.checkpoint_amount) {
        const BoundingBox& b = game.checkpoints[next_checkpoint];

        render_solid(b.as_rect(), {0, 255, 0, 150}, b.get_angle());
    }

    // Fin
    renderer.SetTarget();

    // Ahora si, dibujar el minimapa con un marco, en un cuadrado del 20% de la
    // pantalla en la esquina derecha inferior
    const int mini_width = WIDTH / 5, mini_height = HEIGHT / 5;
    const int start_x = WIDTH - 10 - mini_width;
    const int start_y = HEIGHT - 10 - mini_height;

    render_solid({start_x - 5, start_y - 5, mini_width + 10, mini_height + 10},
                 {0, 0, 0, 255}, false);

    renderer.SetViewport(
        SDL2pp::Rect(start_x, start_y, mini_width, mini_height));
    renderer.Copy(*minimap_texture, SDL2pp::NullOpt, SDL2pp::NullOpt);
    renderer.SetViewport(SDL2pp::NullOpt);
}

void Screen::draw_end_overlay(bool finished) {
    render_solid({0, 0, WIDTH, HEIGHT}, {0, 0, 0, 200}, false);

    if (finished) {
        render_text("Carrera terminada!", {30, 30}, {255, 255, 255, 255},
                    false);
        render_text("Tu tiempo: " + format_time(game.my_car->time), {30, 70},
                    {255, 255, 255, 255}, false);
    } else {
        render_text("Coche destruido...", {30, 30}, {255, 255, 255, 255},
                    false);
    }

    if (game.match_state == MatchState::Racing) {
        render_text("Esperando a que la carrera termine...", {30, 130},
                    {255, 255, 255, 255}, false);
    } else if (game.match_state == MatchState::Intermission) {
        render_text("Elegi una mejora!", {30, 130}, {255, 255, 255, 255},
                    false);

        auto& upgrade = game.upgrade_chosen;
        SDL2pp::Color upgrade_yes(255, 255, 0, 255);
        SDL2pp::Color upgrade_no(255, 255, 255, 255);

        render_text("1 - Mejor Aceleracion", {30, 170},
                    (upgrade == 1) ? upgrade_yes : upgrade_no, false);
        render_text("2 - Mejor Velocidad Maxima", {30, 190},
                    (upgrade == 2) ? upgrade_yes : upgrade_no, false);
        render_text("3 - Mejor Nitro", {30, 210},
                    (upgrade == 3) ? upgrade_yes : upgrade_no, false);
        render_text("4 - Mejor Vida Maxima", {30, 230},
                    (upgrade == 4) ? upgrade_yes : upgrade_no, false);

        // TODO(franco): como muestro el tiempo restante hasta la siguiente,
        // donde esta? render_text("Siguiente carrera en: " +
        // format_time(game.time_left), {30, 300}, false);
    } else if (game.match_state == MatchState::Finished) {
        render_text("Partida terminada!", {30, 130}, {255, 255, 255, 255},
                    false);
    }
}

void Screen::update() {
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
    if (game.my_car) {
        if (game.my_car->finished || game.my_car->disqualified) {
            draw_end_overlay(game.my_car->finished);
        } else {
            draw_hud();
            draw_minimap();
        }
    }

    renderer.SetDrawColor(0, 0, 0, 255);
    renderer.Present();
}
