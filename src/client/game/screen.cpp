#include "client/game/screen.h"

#include <SDL2/SDL.h>

#include <SDL2pp/SDL2pp.hh>
#include <format>
#include <iomanip>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "common/structs.h"

static auto create_upgrade_choices_map(
    const std::vector<UpgradeChoice>& choices) {
    std::unordered_map<UpgradeStat, UpgradeChoice> map;
    for (const auto& choice : choices) {
        map[choice.stat] = choice;
    }
    return map;
}

Screen::Screen(SDL2pp::Renderer& renderer, Game& game, Api& api,
               const CityName& city_name,
               const std::vector<UpgradeChoice>& upgrade_choices)
    : renderer(renderer),
      game(game),
      assets(renderer, city_name),
      api(api),
      upgrade_choices(create_upgrade_choices_map(upgrade_choices)),
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
                         const SDL2pp::Color color, bool in_world,
                         bool centered) {
    SDL2pp::Surface s = assets.font.RenderText_Solid(texto, color);
    SDL2pp::Texture t(renderer, s);

    if (centered) pos -= SDL2pp::Point(t.GetWidth() / 2, t.GetHeight() / 2);
    render(t, pos, 0, in_world);
}

void Screen::render_text_title(const std::string& texto, SDL2pp::Point pos,
                               const SDL2pp::Color color, bool in_world) {
    // Texto grande y siempre centrado
    SDL2pp::Surface s = assets.font_large.RenderText_Solid(texto, color);
    SDL2pp::Texture t(renderer, s);

    pos -= SDL2pp::Point(t.GetWidth() / 2, t.GetHeight() / 2);
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
void Screen::render_explosion(SDL2pp::Point pos, int frame) {
    // 4x4, izquierda a derecha
    if (frame > 4 * 4) return;

    int frame_w = assets.explosion.GetWidth() / 4;
    int frame_h = assets.explosion.GetHeight() / 4;

    int index_x = frame % 4;
    int index_y = frame / 4;

    render_slice(assets.explosion,
                 {frame_w * index_x, frame_h * index_y, frame_w, frame_h}, pos,
                 true);
}

void Screen::render_car(NpcCar& car) {
    SDL2pp::Texture& sprite = *assets.car_name.at(car.type);
    render(sprite, car.pos.get_top_left(), car.pos.get_angle());
}

void Screen::render_car(PlayerCar& car, bool with_name) {
    // Explosion
    if (car.disqualified) {
        auto& explosion = assets.explosion;
        SDL2pp::Point pos =
            car.pos.get_center() - SDL2pp::Point(explosion.GetWidth() / 4 / 2,
                                                 explosion.GetHeight() / 4 / 2);
        render_explosion(pos, explosion_frame[car.id] / 10);

        explosion_frame[car.id] += 1;
        return;
    }
    explosion_frame[car.id] = 0;

    // Coche
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

//
// EVENTOS Y BOTONES
//
void Screen::make_btn_upgrade(const UpgradeStat stat, int x, int y,
                              std::string stat_name) {
    const auto& acc_stat = upgrade_choices.at(stat);
    const auto& [_, delta, penalty] = acc_stat;
    std::ostringstream ss;
    ss << std::fixed << std::setprecision(2);
    ss << "+" << delta << " " << stat_name << " -" << penalty << "s";
    SdlButton btn(renderer, assets.font,
                  SDL2pp::Rect(x, y, UPGRADE_WIDTH, UPGRADE_HEIGHT), ss.str(),
                  [this, stat]() { api.upgrade(stat); });
    buttons.push_back(std::move(btn));
}

void Screen::make_btn_end(int x, int y) {
    SdlButton btn(renderer, assets.font, {x, y, EXIT_WIDTH, EXIT_HEIGHT},
                  "SALIR", [&]() { game.force_exit = true; });
    buttons.push_back(std::move(btn));
}

void Screen::handle_event(SDL_Event& e) {
    for (auto& button : buttons) {
        button.handle_event(e);
    }
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
        render_text("TRAMPOSO", {10, 70}, {255, 255, 0, 255}, false);
    }
    render_text(format_time(game.time_elapsed), {10, 50}, {255, 255, 255, 255},
                false);

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

void Screen::draw_start_timer() {
    render_solid({0, 0, WIDTH, HEIGHT}, {0, 0, 0, 100}, false);

    int time = static_cast<int>(game.time_countdown) + 1;
    std::string title = "Siguiente carrera en " + std::to_string(time) + "...";
    render_text_title(title, {WIDTH / 2, HEIGHT / 2}, {255, 255, 0, 255},
                      false);
}

void Screen::draw_intermission() {
    render_solid({0, 0, WIDTH, HEIGHT}, {0, 0, 0, 200}, false);

    render_text("Carrera terminada!", {WIDTH / 2, 10}, {255, 255, 255, 255},
                false, true);
    render_text("Tu tiempo: " + format_time(game.my_car->time),
                {WIDTH / 2, HEIGHT - 30}, {255, 255, 255, 255}, false, true);

    if (game.match_state == MatchState::Racing) {
        render_text("Esperando a que la carrera termine...", {30, 130},
                    {255, 255, 255, 255}, false);
    } else if (game.match_state == MatchState::Intermission) {
        render_text("Elegi una mejora!", {WIDTH / 2, 50}, {255, 255, 255, 255},
                    false, true);
        // Sumar los botones en la primera instancia.
        if (buttons.empty()) {
            make_btn_upgrade(UpgradeStat::Acceleration,
                             WIDTH * 2 / 5 - UPGRADE_WIDTH, 100, " Acel.");
            make_btn_upgrade(UpgradeStat::MaxSpeed, WIDTH * 3 / 5, 100,
                             " Vel.");
            make_btn_upgrade(UpgradeStat::Nitro, WIDTH * 2 / 5 - UPGRADE_WIDTH,
                             250, " Nitro");
            make_btn_upgrade(UpgradeStat::Health, WIDTH * 3 / 5, 250, " Vida");
        }

        const SDL2pp::Color black = {255, 255, 255, 255};
        const auto& [speed, acc, health, nitro] = game.my_upgrades;
        std::ostringstream ss;
        ss << std::fixed << std::setprecision(2);
        ss << "penalizacion total: " << game.penalty << "s";
        render_text(ss.str(), {WIDTH / 2, HEIGHT - 100}, black, false, true);
        ss.str("");
        ss << "+" << acc << " acc";
        render_text(ss.str(), {WIDTH * 1 / 5, HEIGHT - 70}, black, false, true);
        ss.str("");
        ss << "+" << speed << " vel";
        render_text(ss.str(), {WIDTH * 2 / 5, HEIGHT - 70}, black, false, true);
        ss.str("");
        ss << "+" << nitro << " nitro";
        render_text(ss.str(), {WIDTH * 3 / 5, HEIGHT - 70}, black, false, true);
        ss.str("");
        ss << "+" << health << " vida";
        render_text(ss.str(), {WIDTH * 4 / 5, HEIGHT - 70}, black, false, true);
    } else if (game.match_state == MatchState::Finished) {
        render_text("Partida terminada!", {30, 130}, {255, 255, 255, 255},
                    false);
        if (buttons.empty()) make_btn_end(WIDTH / 2, HEIGHT / 2);
    }
}

void Screen::draw_game_over() {
    render_solid({0, 0, WIDTH, HEIGHT}, {0, 0, 0, 200}, false);
    render_text_title("Coche destruido...", {WIDTH / 2, HEIGHT / 2 - 120},
                      {255, 255, 0, 255}, false);
    if (buttons.empty()) make_btn_end(WIDTH / 2, HEIGHT / 2 + 120);
}

void Screen::update() {
    renderer.Clear();

    update_camera();

    // Manejo de botones
    if (game.match_state != game.old_match_state) buttons.clear();

    // Capa 1: mapa
    draw_ciudad();

    // Capa 2: coches
    draw_coches(RenderLayer::UNDER);
    draw_bridges();
    draw_coches(RenderLayer::OVER);
    draw_overpasses();

    // Capa 3: checkpoints
    draw_next_checkpoint();

    // Capa 4: interfaz
    if (game.my_car) {
        if (game.race_state == RaceState::Countdown) {
            draw_start_timer();
        } else if (game.my_car->finished) {
            draw_intermission();
        } else if (game.my_car->disqualified) {
            draw_game_over();
        } else {
            draw_hud();
            draw_minimap();
        }

        // Capa 5: botones (si aplica)
        for (auto& btn : buttons) {
            btn.render();
        }
    }

    renderer.SetDrawColor(0, 0, 0, 255);
    renderer.Present();
}
