#pragma once

#include <SDL2pp/SDL2pp.hh>
#include <list>

#include "client/game/assets.h"
#include "client/game/car.h"
#include "common/macros.h"
#include "common/structs.h"

class Game;

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
    std::list<SoundInfo> explosions;

    // Reproduccion de efectos
    void try_brake(const PlayerCar& car);
    void try_crash(const PlayerCar& car);
    void try_explosion(const PlayerCar& car);
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

#include "client/game/game.h"
