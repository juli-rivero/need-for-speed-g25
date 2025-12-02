#include "client/game/sound.h"

#include <SDL2/SDL.h>

#include <SDL2pp/SDL2pp.hh>

Sound::Sound(SDL2pp::Mixer& mixer, Game& game, const CityName& city_name)
    : mixer(mixer), game(game), assets(city_name) {
    // Iniciar la musica
    mixer.PlayMusic(*assets.music);
}

//
// AUXILIAR
//
int Sound::get_vol(const PlayerCar& car) const {
    const double dist = car.pos.distance_to(game.cam_x, game.cam_y);
    const int v = MIX_MAX_VOLUME - dist / 3;
    return (v >= 0) ? v : 0;
}

void Sound::try_play(const PlayerCar& car, SDL2pp::Chunk& sound,
                     Sound::SoundPriority priority) {
    // Ver si ya tengo un efecto...
    if (sound_playing.contains(car.id)) {
        Sound::SoundInfo info = sound_playing.at(car.id);

        if (mixer.IsChannelPlaying(info.channel_id) == 0) {
            // Si no esta en uso, removerlo.
            sound_playing.erase(car.id);
        } else if (priority > info.priority) {
            // Esta en uso y es de mayor prioridad, abortarlo.
            mixer.HaltChannel(info.channel_id);
            sound_playing.erase(car.id);
        } else {
            // Es de menor prioridad (o repetido), no reproducirlo.
            return;
        }
    }

    // O no habia ninguno o se aborto el anterior, reproducir el nuevo sonido.
    int channel_id = mixer.PlayChannel(-1, sound);
    mixer.SetVolume(channel_id, get_vol(car));
    sound_playing.emplace(car.id, Sound::SoundInfo(channel_id, priority));
}

//
// EFECTOS
//
void Sound::try_brake(const PlayerCar& car) {
    try_play(car, assets.brake, SoundPriority::BRAKE);
}

void Sound::try_nitro(const PlayerCar& car) {
    try_play(car, assets.nitro, SoundPriority::NITRO);
}

void Sound::try_crash(const PlayerCar& car) {
    try_play(car, assets.crash, SoundPriority::CRASH);
}

void Sound::try_explosion(const PlayerCar& car) {
    try_play(car, assets.explosion, SoundPriority::EXPLOSION);
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
        if (car.speed < 30) continue;

        // Ahora si, intentarlo
        try_brake(car);
    }

    // Descalificaciones
    for (auto& [id, car] : game.cars) {
        // Solo reproducir al inicio del freno
        if (game.old_disqualified[id] == true) continue;
        if (car.disqualified == false) continue;

        // Ahora si, intentarlo
        try_explosion(car);
    }

    // Nitros
    for (auto& [id, car] : game.cars) {
        // No reproducir si esta descalifiado
        if (car.disqualified) continue;

        // Solo reproducir al inicio del nitro
        if (game.old_nitro_active[id] == true) continue;
        if (car.nitro_active == false) continue;

        // Ahora si, intentarlo
        try_nitro(car);
    }

    // Checkpoints
    if (game.my_car == nullptr) return;

    if (game.my_car->finished == true && game.old_finished == false) {
        play_goal();
    } else if (game.my_car->next_checkpoint > game.old_checkpoint) {
        play_checkpoint();
    }

    // (volumen de musica)
    if (game.my_car->finished || game.my_car->disqualified) {
        mixer.SetMusicVolume(MIX_MAX_VOLUME / 4);
    } else {
        mixer.SetMusicVolume(MIX_MAX_VOLUME / 2);
    }
}
