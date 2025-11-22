#include "client/game/sdl_app.h"

#include <SDL2/SDL.h>

#include <SDL2pp/SDL2pp.hh>

#include "client/game/game.h"

SdlApp::SdlApp(Connexion& connexion, bool& quit, GameSetUp& setup) {
    // Evitar que SDL intente usar políticas/valores RT inválidos en Linux.
    // SDL_HINT_THREAD_PRIORITY_POLICY acepta: "current", "other", "fifo", "rr".
    // Usamos "other" (SCHED_OTHER) para mantener prioridades normales y
    // prevenir asserts de glibc/pthreads al cambiar prioridades.
    SDL_SetHint(SDL_HINT_THREAD_PRIORITY_POLICY, "other");
    // Asegurarnos de NO forzar prioridades en tiempo real para hilos marcados
    // como TIME_CRITICAL (audio, mixer, etc.).
    // Valores: "0" por defecto (no forzar), "1" fuerza RT.
    SDL_SetHint(SDL_HINT_THREAD_FORCE_REALTIME_TIME_CRITICAL, "0");

    // Inicializacion de SDL (ventana 640x480)
    SDL2pp::SDL sdl(SDL_INIT_VIDEO);
    SDL2pp::Window window("Need for Speed TPG", SDL_WINDOWPOS_UNDEFINED,
                          SDL_WINDOWPOS_UNDEFINED, 640, 480, 0);
    SDL2pp::Renderer renderer(window, -1, SDL_RENDERER_ACCELERATED);

    SDL2pp::SDLTTF sdl_ttf;

    SDL2pp::Mixer mixer(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT,
                        MIX_DEFAULT_CHANNELS, 4096);

    // Iniciar partida
    Game game(renderer, mixer, connexion, setup);
    quit = game.start();
}

SdlApp::~SdlApp() {}
