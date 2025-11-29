#include <SDL2/SDL.h>

#include <SDL2pp/SDL2pp.hh>

#include "client/game/classes.h"
#include "spdlog/spdlog.h"

Sound::Sound(SDL2pp::Mixer& mixer, Game& game) : mixer(mixer), game(game) {}

int Sound::get_vol(const Car& car) const {
    const double dx = game.cam_world_x - car.x;
    const double dy = game.cam_world_y - car.y;

    const double dist = std::sqrt(dx * dx + dy * dy);

    const int v = MIX_MAX_VOLUME - dist / 3;
    return (v >= 0) ? v : 0;
}

void Sound::crash(const Car& car) const {
    spdlog::info("choque con {}", car.id);

    mixer.PlayChannel(car.id, game.assets.sound_crash);
    mixer.SetVolume(car.id, get_vol(car));
}

void Sound::test_brake(const Car& car) {
    // TODO(franco): limite de velocidad minimo para el sonido
    if (car.braking == true && old_braking == false) {
        mixer.PlayChannel(car.id, game.assets.sound_brake);
        mixer.SetVolume(car.id, get_vol(car));
    }

    old_braking = car.braking;
}

void Sound::test_finish(const Car& car) {
    if (car.finished == true && old_finished == false) {
        mixer.PlayChannel(-1, game.assets.sound_finish);
    }

    old_finished = car.finished;
}
