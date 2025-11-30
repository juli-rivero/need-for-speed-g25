// FIXME(franco): varios errores de dependencia mutua me forzaron a unir todas
// las clases, luego replantear como modularizar el codigo correctamente... por
// ahora, "make it work, make it right, make it fast"

#pragma once

#include <SDL2pp/SDL2pp.hh>
#include <list>
#include <string>
#include <unordered_map>
#include <vector>

#include "client/connexion/connexion.h"
#include "client/constants.h"
#include "client/game/assets.h"
#include "client/game/bounding_box.h"
#include "client/game/car.h"
#include "common/macros.h"
#include "common/structs.h"

class Game;

class Screen final {
   private:
    SDL2pp::Renderer& renderer;
    Game& game;

    PlayerCar* my_car = nullptr;

    // Constantes utiles
    const int WIDTH;
    const int HEIGHT;

    // Pasos de renderizado
    void draw_ciudad();
    void draw_next_checkpoint();
    void draw_coches(RenderLayer capa);
    void draw_bridges();
    void draw_overpasses();
    void draw_hud();

    // Metodos de renderizado
    //
    // Con in_world == true se dibuja relativo al mundo, no la pantalla.
    // Modificar cam_x e cam_y para impactar donde dibujar.
    int cam_offset_x = 0;
    int cam_offset_y = 0;
    void update_camera();

    void render(SDL2pp::Texture& surface, SDL2pp::Point pos, double angle = 0,
                bool in_world = true);

    void render_slice(SDL2pp::Texture& texture, SDL2pp::Rect section,
                      SDL2pp::Point pos, bool in_world = true);

    void render_text(const std::string& texto, SDL2pp::Point pos,
                     bool in_world = true);

    void render_solid(SDL2pp::Rect rect, const SDL2pp::Color& color,
                      bool in_world = true);
    void render_solid(SDL2pp::Rect rect, const SDL2pp::Color& color,
                      double angle, bool in_world = true);

    // Metodos de renderizado compuestos
    //
    // Utilizan los metodos definidos arriba.
    void render_car(NpcCar& car);
    void render_car(PlayerCar& car, bool with_name);

   public:
    Screen(SDL2pp::Renderer& renderer, Game& game);

    void update();

    MAKE_FIXED(Screen)
};

class Sound final {
   private:
    SDL2pp::Mixer& mixer;
    Game& game;

    bool old_braking = false;
    bool old_finished = false;

    int get_vol(const PlayerCar& car) const;

   public:
    Sound(SDL2pp::Mixer& mixer, Game& game);

    void crash(const PlayerCar& car) const;
    void test_brake(const PlayerCar& car);
    void test_finish(const PlayerCar& car);

    MAKE_FIXED(Sound)
};

class Game final : Connexion::Responder {
    friend class Car;
    friend class Screen;
    friend class Sound;

   private:
    // Componentes de conexion y configuracion
    Api& api;
    const PlayerId my_id;

    // Componentes visuales y auditivos
    Screen screen;
    Sound sound;
    Assets assets;

    // Componentes de snapshot estatico
    const CityInfo& city_info;
    const RaceInfo& race_info;

    std::list<BoundingBox> bridges;
    std::list<BoundingBox> overpasses;
    std::vector<BoundingBox> checkpoints;
    size_t checkpoint_amount;

    // Componentes de snapshot de actualizacion
    std::mutex snapshot_mutex;
    GameSnapshot current_snapshot;
    void on_game_snapshot(const GameSnapshot&) override;

    std::unordered_map<PlayerId, PlayerCar> cars;
    std::list<NpcCar> npcs;
    PlayerCar* my_car = nullptr;

    float time_elapsed = 0;
    float time_countdown = 0;
    float time_left = 0;

    // Componentes de colisiones recibidas
    Queue<CollisionEvent> collisions;
    void on_collision_event(const CollisionEvent& collision_event) override;

    // Componentes de camara a usar
    int cam_x = 0;
    int cam_y = 0;
    void set_camera(const PlayerCar& car);

    // Pasos de actualizacion
    bool send_events();
    void update_state();
    void manage_sounds();

   public:
    Game(SDL2pp::Renderer& renderer, SDL2pp::Mixer& mixer, Connexion& connexion,
         const GameSetUp& setup);
    ~Game();

    bool start();

    MAKE_FIXED(Game)
};
