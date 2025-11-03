#pragma once

#include <QObject>
#include <QString>
#include <vector>

// Estructura para representar una partida
struct GameInfo {
    int id;
    QString name;
    QString map;
    int currentPlayers;
    int maxPlayers;
    QString status;  // "waiting", "playing", "full"
};

// Estructura para crear una partida
struct GameConfig {
    QString name;
    int maxPlayers;
    int raceCount;
    int lapCount;
    QString city;
    int carType;
};

// Estructura para representar un jugador en la sala de espera
struct PlayerInfo {
    int id;
    QString name;
    int carType;
    bool isReady;
    bool isHost;
};

/**
 * Interfaz abstracta para comunicación con el servidor
 * Esto permite que el Lobby use esta interfaz sin importar
 * cómo Juli implemente el protocolo
 */
class ILobbyClient : public QObject {
    Q_OBJECT

   public:
    virtual ~ILobbyClient() = default;

    // MÉTODOS QUE DEBE IMPLEMENTAR EL PROTOCOLO REAL
    virtual void connectToServer(const QString& host, int port) = 0;
    virtual void requestGamesList() = 0;
    virtual void createGame(const GameConfig& config) = 0;
    virtual void joinGame(int gameId) = 0;
    virtual void leaveGame() = 0;
    virtual void setReady(bool ready) = 0;
    virtual void disconnect() = 0;

    // TODO(juli): Agregar estos métodos cuando sea necesario:
    // virtual void selectCar(int gameId, const CarConfig& car) = 0;
    // virtual void playerReady(int gameId) = 0;
    // virtual void leaveGame(int gameId) = 0;

   signals:
    // Emitir estas señales cuando se reciban respuestas del servidor
    void connected();
    void disconnected();
    void connectionError(QString error);
    void gamesListReceived(std::vector<GameInfo> games);
    void gameCreated(int gameId);
    void gameJoined(int gameId);
    void playersListUpdated(std::vector<PlayerInfo> players);
    void gameStarting();
    void error(QString message);

    // TODO(juli): Agregar señales adicionales cuando sea necesario:
    // void carSelected(int gameId);
    // void gameStarting(int gameId, /* datos del juego */);
    // void playerLeft(int gameId, int playerId);
};

/**
 * Implementación MOCK para desarrollo
 * Simula las respuestas del servidor con datos hardcodeados
 *
 * ESTA CLASE ES TEMPORAL
 * Más tarde Juli reemplazará esto con la implementación real (RealLobbyClient)
 */
class MockLobbyClient : public ILobbyClient {
    Q_OBJECT

   public:
    MockLobbyClient();

    void connectToServer(const QString& host, int port) override;
    void requestGamesList() override;
    void createGame(const GameConfig& config) override;
    void joinGame(int gameId) override;
    void leaveGame() override;
    void setReady(bool ready) override;
    void disconnect() override;

   private:
    std::vector<GameInfo> mockGames;
    std::vector<PlayerInfo> mockPlayers;
    void initMockData();
};
