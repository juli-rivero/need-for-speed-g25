#pragma once

#include <SDL2pp/SDL2pp.hh>
#include <memory>
#include <string>
#include <unordered_map>

#include "client/game/assets.h"
#include "client/game/car.h"
#include "common/macros.h"
#include "common/structs.h"

class Game;

class Screen final {
   private:
    SDL2pp::Renderer& renderer;
    Game& game;
    AssetsScreen assets;

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
    void draw_end_overlay(bool finished);

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

    void render_text(const std::string& texto, SDL2pp::Point pos,
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
    Screen(SDL2pp::Renderer& renderer, Game& game, const CityName& city_name);

    void update();

    MAKE_FIXED(Screen)
};

#include "client/game/game.h"
