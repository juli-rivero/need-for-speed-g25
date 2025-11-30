#include <SDL2/SDL.h>

#include <SDL2pp/SDL2pp.hh>

#include "client/game/classes.h"
#include "spdlog/spdlog.h"

Sound::Sound(SDL2pp::Mixer& mixer, Game& game) : mixer(mixer), game(game) {}

int Sound::get_vol(const PlayerCar& car) const {
    const double dist = car.pos.distance_to(game.cam_x, game.cam_y);
    const int v = MIX_MAX_VOLUME - dist / 3;
    return (v >= 0) ? v : 0;
}

void Sound::crash(const PlayerCar& car) const {
    mixer.PlayChannel(car.id, game.assets.sound_crash);
    mixer.SetVolume(car.id, get_vol(car));
}

void Sound::test_brake(const PlayerCar& car) {
    // TODO(franco): limite de velocidad minimo para el sonido
    if (car.braking == true && old_braking == false) {
        mixer.PlayChannel(car.id, game.assets.sound_brake);
        mixer.SetVolume(car.id, get_vol(car));
    }

    old_braking = car.braking;
}

void Sound::test_finish(const PlayerCar& car) {
    if (car.finished == true && old_finished == false) {
        mixer.PlayChannel(-1, game.assets.sound_finish);
    }

    old_finished = car.finished;
}
