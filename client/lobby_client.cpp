#include "lobby_client.h"
#include <QTimer>

MockLobbyClient::MockLobbyClient() {
    initMockData();
}

void MockLobbyClient::connectToServer(const QString& host, int port) {
    Q_UNUSED(host);
    Q_UNUSED(port);
    
    // Simular delay de conexión
    QTimer::singleShot(500, this, [this]() {
        emit connected();
    });
}

void MockLobbyClient::requestGamesList() {
    // Simular delay de red
    QTimer::singleShot(300, this, [this]() {
        emit gamesListReceived(mockGames);
    });
}

void MockLobbyClient::createGame(const GameConfig& config) {
    // Simular creación de partida
    QTimer::singleShot(400, this, [this, config]() {
        GameInfo newGame;
        newGame.id = mockGames.size() + 1;
        newGame.name = config.name;
        newGame.map = config.city;
        newGame.currentPlayers = 1;
        newGame.maxPlayers = config.maxPlayers;
        newGame.status = "waiting";
        
        mockGames.push_back(newGame);
        emit gameCreated(newGame.id);
    });
}

void MockLobbyClient::joinGame(int gameId) {
    Q_UNUSED(gameId);
    
    // Simular unirse a partida
    QTimer::singleShot(300, this, [this, gameId]() {
        // Actualizar mock data (incrementar jugadores)
        for (auto& game : mockGames) {
            if (game.id == gameId && game.currentPlayers < game.maxPlayers) {
                game.currentPlayers++;
                break;
            }
        }
        emit gameJoined(gameId);
    });
}

void MockLobbyClient::disconnect() {
    emit disconnected();
}

void MockLobbyClient::initMockData() {
    // Datos hardcodeados para desarrollo
    mockGames = {
        {1, "Mario Circuit", "city", 3, 4, "waiting"},
    };
}
