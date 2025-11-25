// FIXME(franco): varios errores de dependencia mutua...
// poner todos las definiciones en uno en serio es la mejor solucion?

#pragma once

#include <SDL2pp/SDL2pp.hh>
#include <string>
#include <unordered_map>
#include <vector>

#include "client/connexion/connexion.h"
#include "client/constants.h"
#include "common/macros.h"

class Assets final {
   public:
    explicit Assets(SDL2pp::Renderer& renderer);

    // Assets a secas
    SDL2pp::Texture car_speedster;
    SDL2pp::Texture car_tank;
    SDL2pp::Texture car_drifter;
    SDL2pp::Texture car_rocket;
    SDL2pp::Texture car_classic;
    SDL2pp::Texture car_offroad;
    SDL2pp::Texture car_ghost;
    SDL2pp::Texture city_liberty;
    SDL2pp::Chunk sound_brake;
    SDL2pp::Chunk sound_crash;
    SDL2pp::Chunk sound_finish;
    SDL2pp::Font font;

    SDL2pp::Texture arrow;

    // Mapeos de cadena
    std::unordered_map<CarSpriteType, SDL2pp::Texture*> car_name;
    std::unordered_map<std::string, SDL2pp::Texture*> city_name;

    MAKE_FIXED(Assets)
};

class Game;

class Car final {
   private:
    Game& game;
    SDL2pp::Texture& sprite;

    // AUXILIAR: Conseguir el volumen de un sonido de este car, relativo a la
    // pantalla.
    int get_vol() const;

   public:
    Car(Game& game, const PlayerSnapshot& base);

    // Parametros
    const int id;
    const std::string name;
    const float x;
    const float y;
    const float angle;
    const float speed;
    const float health;
    const uint32_t next_checkpoint;

    // Constantes utiles
    const int WIDTH;
    const int HEIGHT;

    void set_camera();
    void sound_crash();
    void draw(bool with_name);

    // Calcula el angulo desde su centro al centro del siguiente checkpoint
    // Setea has_angle relativo a si el angulo existe
    float get_angle_to_next_checkpoint(bool& has_angle) const;

    MAKE_FIXED(Car)
};

class Screen final {
   private:
    SDL2pp::Renderer& renderer;
    Game& game;

    Car* my_car = nullptr;

    // Pasos de renderizado
    void draw_ciudad();
    void draw_checkpoint();
    void draw_coches();
    void draw_hud();

   public:
    Screen(SDL2pp::Renderer& renderer, Game& game);

    // Constantes utiles
    const int WIDTH;
    const int HEIGHT;

    void update();

    // Metodos de renderizado
    //
    // Con in_world == true se dibuja relativo al mundo, no la pantalla.
    // Modificar cam_x e cam_y para impactar donde dibujar.
    // TODO(franco): migrar la camara a screen.
    // TODO(franco): esto no deberia ser private? refactorizar car?
    void render(SDL2pp::Texture& surface, int x, int y, double angle = 0,
                bool in_world = true);
    void render(const std::string& texto, int x, int y, bool in_world = true);
    void render_rect(SDL2pp::Rect rect, const SDL2pp::Color& color,
                     bool in_world = true);

    MAKE_FIXED(Screen)
};

class Game final : Connexion::Responder {
    friend class Car;
    friend class Screen;

   private:
    // Componentes graficos
    Screen screen;
    SDL2pp::Mixer& mixer;
    Assets assets;

    SDL2pp::Texture& city;

    // Componentes de conexion
    Api& api;
    const PlayerId my_id;
    const GameSetUp& setup;

    // Componentes de snapshot update
    std::mutex snapshot_mutex;

    float time_elapsed = 0;
    std::vector<PlayerSnapshot> player_snapshots;
    void on_game_snapshot(
        const float time_elapsed,
        const std::vector<PlayerSnapshot>& player_snapshots) override;

    std::unordered_map<PlayerId, Car> cars;
    Car* my_car = nullptr;

    // Componentes de colisiones
    Queue<CollisionEvent> collisions;
    void on_collision_event(const CollisionEvent& collision_event) override;

    // Metodos de actualizacion internos
    bool send_events();
    void update_state();
    void manage_collisions();

    // TODO(franco): migrar la camara a screen.
    int cam_x = 0;
    int cam_y = 0;
    double cam_world_x = 0;
    double cam_world_y = 0;

   public:
    Game(SDL2pp::Renderer& renderer, SDL2pp::Mixer& mixer, Connexion& connexion,
         const GameSetUp& setup);
    ~Game();

    bool start();

    MAKE_FIXED(Game)
};
