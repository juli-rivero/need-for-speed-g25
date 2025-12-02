#pragma once

#include <SDL2pp/SDL2pp.hh>
#include <list>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "client/connexion/sender.h"
#include "client/game/assets.h"
#include "client/game/car.h"
#include "client/game/sdl_button.h"
#include "common/macros.h"
#include "common/structs.h"

class Game;

class Screen final {
    SDL2pp::Renderer& renderer;
    Game& game;
    AssetsScreen assets;

    Api& api;

    std::unordered_map<UpgradeStat, UpgradeChoice> upgrade_choices;

    // Constantes utiles
    const int WIDTH;
    const int HEIGHT;

    // Auxiliares de renderizado
    std::unique_ptr<SDL2pp::Texture> minimap_texture;
    std::unordered_map<PlayerId, int> explosion_frame;

    // Pasos de renderizado
    void draw_ciudad();
    void draw_next_checkpoint();
    void draw_coches(RenderLayer capa);
    void draw_bridges();
    void draw_overpasses();
    void draw_hud();
    void draw_minimap();
    void draw_start_timer();
    void draw_intermission();
    void draw_game_over();

    // Eventos
    const int UPGRADE_WIDTH = 200, UPGRADE_HEIGHT = 100;
    void make_btn_upgrade(UpgradeStat stat, int x, int y,
                          std::string stat_name);

    const int EXIT_WIDTH = 100, EXIT_HEIGHT = 100;
    void make_btn_end(int x, int y);

    std::list<SdlButton> buttons;

    // Metodos de renderizado
    //
    // Con in_world == true se dibuja relativo al mundo, no la pantalla.
    // Modificar game.cam_x e game.cam_y para impactar donde dibujar.
    int cam_offset_x = 0;
    int cam_offset_y = 0;
    void update_camera();

    void render(SDL2pp::Texture& surface, SDL2pp::Point pos, double angle = 0,
                bool in_world = true);

    void render_slice(SDL2pp::Texture& texture, SDL2pp::Rect section,
                      SDL2pp::Point pos, bool in_world = true);

    // Texto de tipo titulo: mas grande y siempre centrado
    void render_text(const std::string& texto, SDL2pp::Point pos,
                     const SDL2pp::Color color, bool in_world = true,
                     bool centered = false);
    void render_text_title(const std::string& texto, SDL2pp::Point pos,
                           const SDL2pp::Color color, bool in_world = true);

    void render_solid(SDL2pp::Rect rect, const SDL2pp::Color& color,
                      bool in_world = true);
    void render_solid(SDL2pp::Rect rect, const SDL2pp::Color& color,
                      double angle, bool in_world = true);

    // Metodos de renderizado compuestos
    //
    // Utilizan los metodos definidos arriba.
    void render_explosion(SDL2pp::Point pos, int frame);
    void render_car(NpcCar& car);
    void render_car(PlayerCar& car, bool with_name);

   public:
    Screen(SDL2pp::Renderer& renderer, Game& game, Api& api,
           const CityName& city_name, const std::vector<UpgradeChoice>&);

    void update();
    void handle_event(SDL_Event& e);

    MAKE_FIXED(Screen)
};

#include "client/game/game.h"
