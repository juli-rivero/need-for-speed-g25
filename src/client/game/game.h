#pragma once

#include <SDL2pp/SDL2pp.hh>
#include <list>
#include <string>
#include <vector>

#include "client/connexion/connexion.h"
#include "client/constants.h"
#include "client/game/assets.h"
#include "client/game/car.h"
#include "common/macros.h"

class Car;

class Game final : Connexion::Responder {
    friend class Car;

   private:
    // Componentes graficos
    SDL2pp::Renderer& renderer;
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

    std::list<Car> cars;
    Car* my_car{nullptr};

    // Componentes de colisiones
    Queue<CollisionEvent> collisions;
    void on_collision_event(const CollisionEvent& collision_event) override;

    // Metodos de actualizacion internos
    bool send_events();
    void update_state();
    void manage_collisions();
    void draw_state();

    // Auxiliar: dibujar una textura (o texto) en la ubicacion dada.
    // Con in_world == true, se dibuja relativo al mundo, no relativo a la
    // pantalla En ese caso, modificar las coordenadas de la camara para
    // impactar el punto base usado.
    int cam_x = 0;
    int cam_y = 0;
    double cam_world_x = 0;
    double cam_world_y = 0;

    void render(SDL2pp::Texture& surface, int x, int y, double angle = 0,
                bool in_world = true);
    void render(const std::string& texto, int x, int y, bool in_world = true);
    void render_rect(SDL2pp::Rect rect, const SDL2pp::Color& color,
                     bool in_world = true);

   public:
    explicit Game(SDL2pp::Renderer& renderer, SDL2pp::Mixer& mixer,
                  Connexion& connexion, const GameSetUp& setup);
    ~Game();

    bool start();

    MAKE_FIXED(Game)
};
