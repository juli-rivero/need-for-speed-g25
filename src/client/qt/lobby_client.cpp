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

void MockLobbyClient::leaveGame() {
    // Simular salir de la partida
    QTimer::singleShot(200, this, [this]() {
        emit gamesListReceived(mockGames);
    });
}

void MockLobbyClient::setReady(bool ready) {
    // Actualizar estado ready del jugador actual (índice 0)
    if (!mockPlayers.empty()) {
        mockPlayers[0].isReady = ready;
    }
    
    // Simular actualización de la lista
    QTimer::singleShot(300, this, [this]() {
        emit playersListUpdated(mockPlayers);
        
        // Si todos están listos, iniciar partida después de 2 segundos
        bool allReady = true;
        for (const auto& p : mockPlayers) {
            if (!p.isReady) {
                allReady = false;
                break;
            }
        }
        
        if (allReady && mockPlayers.size() >= 2) {
            QTimer::singleShot(2000, this, [this]() {
                emit gameStarting();
            });
        }
    });
}

void MockLobbyClient::initMockData() {
    // Datos hardcodeados para desarrollo
    mockGames = {
        {1, "Mario Circuit", "city", 3, 4, "waiting"},
        {2, "Rainbow Road", "city", 2, 6, "waiting"},
        {3, "Tokyo Drift", "city", 4, 4, "full"},
    };
    
    // Jugadores mock iniciales (se actualizan cuando alguien se une)
    mockPlayers = {
        {1, "Tú", 0, false, true},      // El jugador actual (host)
        {2, "Jugador 2", 1, false, false},
        {3, "Jugador 3", 2, true, false},
    };
}
