#pragma once

#ifndef GAME_FORWARD_DECL
#define GAME_FORWARD_DECL
class Game;
#endif

#include <SDL2pp/SDL2pp.hh>
#include <list>
#include <unordered_map>
#include <vector>

#include "client/connexion/connexion.h"
#include "client/constants.h"
#include "client/game/bounding_box.h"
#include "client/game/car.h"
#include "client/game/screen.h"
#include "client/game/sound.h"
#include "common/macros.h"
#include "common/structs.h"

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
    // Queue<CollisionEvent> collisions; (public)
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

    std::list<BoundingBox> walls;
    std::list<BoundingBox> bridges;
    std::list<BoundingBox> overpasses;
    std::vector<BoundingBox> checkpoints;
    size_t checkpoint_amount;

    // Componentes parseados de snapshot de actualizacion (visual)
    std::unordered_map<PlayerId, PlayerCar> cars;
    std::list<NpcCar> npcs;
    PlayerCar* my_car = nullptr;
    UpgradeStats my_upgrades;

    float time_elapsed = 0;
    float time_countdown = 0;
    float time_left = 0;
    RaceState race_state = RaceState::Countdown;
    MatchState match_state = MatchState::Starting;

    // Componentes parseados de snapshot de actualizacion (sonido)
    Queue<CollisionEvent> collisions;

    std::unordered_map<PlayerId, bool> old_braking;
    std::unordered_map<PlayerId, bool> old_disqualified;
    uint32_t old_checkpoint = 0;
    bool old_finished = false;

    // Componentes miscelaneos
    bool cheat_mode = false;
    bool cheat_used = false;

    // Componentes de camara a usar
    int cam_x = 0;
    int cam_y = 0;
    void set_camera(const PlayerCar& car);

    // Metodo de inicio.
    bool start();

    MAKE_FIXED(Game)
};
