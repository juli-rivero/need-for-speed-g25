#include "client/game/game.h"

#include <SDL2pp/SDL2pp.hh>
#include <SDL2/SDL.h>

#define ASSETS_PATH ""

// TODO: Remover esto, lo incluyo solamente por integracion.
using namespace SDL2pp;

bool Game::start() {
	// Inicializacion de SDL (640x480)
	SDL sdl(SDL_INIT_VIDEO);
	Window window("Need for Speed TPG",
			SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
			640, 480, 0);
	Renderer renderer(window, -1, SDL_RENDERER_ACCELERATED);

	// Cargar las texturas a usar
	//Surface car_surface(ASSETS_PATH "/cars/car1.png");
	//car_surface.SetColorKey(true, SDL_MapRGB((SDL_PixelFormat)car_surface.GetFormat(), 0xA3, 0xA3, 0x0D));
	
	// TODO: DEFINIR USO DE ASSETS.
	Texture car_sprite(renderer, ASSETS_PATH "/cars/car1.png");
	Texture city_sprite(renderer, ASSETS_PATH "/cities/liberty_city.png");
	
	// Game loop
	double car_x = renderer.GetOutputWidth()  / 2;
	double car_y = renderer.GetOutputHeight() / 2;
	
	double car_speed = 0;
	double car_angle = 0;
	
	while (1) {
		// Manejo de inputs
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) return false;
			
			if (event.type == SDL_KEYDOWN) {
				auto tecla = event.key.keysym.sym;
				
				if (tecla == SDLK_LEFT)  car_angle -= 10;
				if (tecla == SDLK_RIGHT) car_angle += 10;
				if (tecla == SDLK_UP)    car_speed -= 1;
				if (tecla == SDLK_DOWN)  car_speed += 1;
			}
		}
		
		if (car_angle > 360) car_angle -= 360;
		if (car_angle < 0)   car_angle += 360;
		car_x += sin(-car_angle * 3.14 / 180) * car_speed;
		car_y += cos(-car_angle * 3.14 / 180) * car_speed;
		
		// Renderizado del siguiente frame
		renderer.Clear();
		renderer.Copy(city_sprite, Rect(100, 100, renderer.GetOutputWidth(), renderer.GetOutputHeight()), Point(0, 0));
		renderer.Copy(car_sprite, NullOpt, Point(car_x, car_y), car_angle);
		renderer.Present();
		
		// Demorar para "en teoria" 60 FPS (en realidad no)
		SDL_Delay(1000 / 60);
	}
	
	return false;
}
