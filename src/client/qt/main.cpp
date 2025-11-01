#include "common/foo.h"
#include "client/lobby_window.h"

#include <iostream>
#include <exception>

#include <QApplication>
#include <SDL2pp/SDL2pp.hh>
#include <SDL2/SDL.h>

using namespace SDL2pp;

int main(int argc, char *argv[]) try {
	// Inicializar aplicación Qt
	QApplication app(argc, argv);

	// Crear y mostrar ventana del lobby
	LobbyWindow lobby;
	lobby.show();

	// Ejecutar el loop de eventos de Qt
	return app.exec();

	// TODO: Después de salir del lobby, iniciar el juego con SDL2
	// Esto se implementará cuando se integre con el protocolo

} catch (std::exception& e) {
	// Si hay error, imprimirlo y salir con código de error
	std::cerr << e.what() << std::endl;
	return 1;
}
