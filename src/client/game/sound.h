#pragma once

#ifndef GAME_FORWARD_DECL
#define GAME_FORWARD_DECL
class Game;
#endif

#include <SDL2pp/SDL2pp.hh>
#include <unordered_map>

#include "client/game/assets.h"
#include "client/game/car.h"
#include "common/macros.h"
#include "common/structs.h"

class Sound final {
   private:
    SDL2pp::Mixer& mixer;
    Game& game;
    AssetsSound assets;

    // Prioridad de sonidos (mas alto = sobreescribe el anterior)
    enum SoundPriority {
        BRAKE = 1,
        NITRO = 2,
        CRASH = 3,
        EXPLOSION = 4
        // CHECKPOINT y GOAL siempre se reproducen.
    };

    // Manejo interno de sonidos
    struct SoundInfo {
        int channel_id;
        SoundPriority priority;
    };
    std::unordered_map<PlayerId, SoundInfo> sound_playing;

    // Reproduccion de efectos
    void try_brake(const PlayerCar& car);
    void try_crash(const PlayerCar& car);
    void try_explosion(const PlayerCar& car);
    void try_nitro(const PlayerCar& car);
    void play_checkpoint();
    void play_goal();

    // Auxiliares de reproduccion
    int get_vol(const PlayerCar& car) const;
    void try_play(const PlayerCar& car, SDL2pp::Chunk& sound,
                  SoundPriority priority);

   public:
    Sound(SDL2pp::Mixer& mixer, Game& game, const CityName& city_name);

    // Metodo principal de actualizacion
    void update();

    MAKE_FIXED(Sound)
};

#include "client/game/game.h"
