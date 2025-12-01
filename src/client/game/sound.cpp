#include <SDL2/SDL.h>

#include <SDL2pp/SDL2pp.hh>

#include "client/game/classes.h"

Sound::Sound(SDL2pp::Mixer& mixer, Game& game, const CityName& city_name)
    : mixer(mixer), game(game), assets(city_name) {}

//
// AUXILIAR
//
int Sound::get_vol(const PlayerCar& car) const {
    const double dist = car.pos.distance_to(game.cam_x, game.cam_y);
    const int v = MIX_MAX_VOLUME - dist / 3;
    return (v >= 0) ? v : 0;
}

void Sound::try_play(const PlayerCar& car, std::list<SoundInfo>& info,
                     SDL2pp::Chunk& sound) {
    // Si ya esta reproduciendo un sonido este car, no reproducirlo.
    // Adicionalmente, remover canales que no esten en uso.
    bool already_playing = false;
    for (auto i = info.begin(); i != info.end();) {
        const SoundInfo& current = *i;

        if (current.from == car.id) already_playing = true;

        if (mixer.IsChannelPlaying(current.channel_id) == 0) {
            i = info.erase(i);
            already_playing = false;
        } else {
            i++;
        }
    }
    if (already_playing) return;

    // Si hay mas de tres sonidos ya reproduciendose, no sumar otro
    if (info.size() >= 3) return;

    // Ahora si, reproducirlo y sumarlo.
    int channel_id = mixer.PlayChannel(-1, sound);
    mixer.SetVolume(channel_id, get_vol(car));
    info.emplace_back(car.id, channel_id);
}

void Sound::finish() {
    // Sonido de victoria
    play_goal();

    // Atenuar la musica
    mixer.SetMusicVolume(MIX_MAX_VOLUME / 4);
}

//
// EFECTOS
//
void Sound::try_brake(const PlayerCar& car) {
    try_play(car, brakes, assets.brake);
}

void Sound::try_crash(const PlayerCar& car) {
    try_play(car, crashes, assets.crash);
}

void Sound::play_checkpoint() {
    // Reproducir globalmente, solo lo origino de mi propio car.
    int channel_id = mixer.PlayChannel(-1, assets.checkpoint);
    mixer.SetVolume(channel_id, MIX_MAX_VOLUME);
}

void Sound::play_goal() {
    // Reproducir globalmente, solo lo origino de mi propio car.
    int channel_id = mixer.PlayChannel(-1, assets.goal);
    mixer.SetVolume(channel_id, MIX_MAX_VOLUME);
}

//
// METODOS PRINCIPALES
//
void Sound::start() {
    // Iniciar (o reiniciar la musica)
    if (!mixer.IsMusicPlaying()) {
        mixer.PlayMusic(*assets.music);
    }
    mixer.SetMusicVolume(MIX_MAX_VOLUME / 2);
}

void Sound::update() {
    // Colisiones
    CollisionEvent collision;
    while (game.collisions.try_pop(collision)) {
        // Conseguir a cual corresponde
        PlayerId id;
        std::visit([&](const auto& collision) { id = collision.player; },
                   collision);

        PlayerCar& car = game.cars.at(id);

        // Pero si esta descalificado no hacerlo
        if (car.disqualified) continue;

        // Ahora si, intentarlo
        try_crash(car);
    }

    // Frenos
    for (auto& [id, car] : game.cars) {
        // No reproducir si esta descalifiado
        if (car.disqualified) continue;

        // Solo reproducir al inicio del freno
        if (game.old_braking[id] == true) continue;
        if (car.braking == false) continue;

        // Y solo si esta yendo lo suficientemente rapido
        if (car.speed < 40) continue;

        // Ahora si, intentarlo
        try_brake(car);
    }

    // Descalificaciones

    // Checkpoints / final
    if (game.my_car == nullptr) return;

    if (game.my_car->finished == true && game.old_finished == false) {
        finish();
    } else if (game.my_car->next_checkpoint > game.old_checkpoint) {
        play_checkpoint();
    }
}
