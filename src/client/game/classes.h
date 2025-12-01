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
    AssetsScreen assets;

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
    Screen(SDL2pp::Renderer& renderer, Game& game, const CityName& city_name);

    void update();

    MAKE_FIXED(Screen)
};

class Sound final {
   private:
    SDL2pp::Mixer& mixer;
    Game& game;
    AssetsSound assets;

    // Manejo interno de sonidos
    struct SoundInfo {
        PlayerId from;
        int channel_id;
    };
    std::list<SoundInfo> brakes;
    std::list<SoundInfo> crashes;

    // Reproduccion de efectos
    void try_brake(const PlayerCar& car);
    void try_crash(const PlayerCar& car);
    void play_checkpoint();
    void play_goal();

    // Finalizacion interna (por update)
    void finish();

    // Auxiliares de reproduccion
    int get_vol(const PlayerCar& car) const;
    void try_play(const PlayerCar& car, std::list<SoundInfo>& info,
                  SDL2pp::Chunk& sound);

   public:
    Sound(SDL2pp::Mixer& mixer, Game& game, const CityName& city_name);

    // Metodos de control
    void start();
    void update();

    MAKE_FIXED(Sound)
};

class Game final : Connexion::Responder {
   private:
    // Componentes de conexion y configuracion
    Api& api;
    const CityInfo& city_info;
    const RaceInfo& race_info;

    // Componentes visuales y auditivos
    Screen screen;
    Sound sound;

    // Componentes de snapshot de actualizacion
    std::mutex snapshot_mutex;
    GameSnapshot current_snapshot;
    void on_game_snapshot(const GameSnapshot&) override;

    // Componentes de colisiones recibidas
    // Queue<CollisionEvent> collisions;
    void on_collision_event(const CollisionEvent& collision_event) override;

    // Pasos de actualizacion
    bool send_events();
    void update_state();
    void manage_sounds();

   public:
    Game(SDL2pp::Renderer& renderer, SDL2pp::Mixer& mixer, Connexion& connexion,
         const GameSetUp& setup);
    ~Game();

    // Componentes de snapshot estatico
    const PlayerId my_id;

    std::list<BoundingBox> bridges;
    std::list<BoundingBox> overpasses;
    std::vector<BoundingBox> checkpoints;
    size_t checkpoint_amount;

    // Componentes parseados de snapshot de actualizacion (visual)
    std::unordered_map<PlayerId, PlayerCar> cars;
    std::list<NpcCar> npcs;
    PlayerCar* my_car = nullptr;

    float time_elapsed = 0;
    float time_countdown = 0;
    float time_left = 0;

    // Componentes parseados de snapshot de actualizacion (sonido)
    Queue<CollisionEvent> collisions;

    std::unordered_map<PlayerId, bool> old_braking;
    std::unordered_map<PlayerId, bool> old_disqualified;
    uint32_t old_checkpoint = 0;
    bool old_finished = false;

    // Componentes de camara a usar
    int cam_x = 0;
    int cam_y = 0;
    void set_camera(const PlayerCar& car);

    // Metodo de inicio.
    bool start();

    MAKE_FIXED(Game)
};
