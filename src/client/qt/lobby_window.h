#ifndef LOBBY_WINDOW_H
#define LOBBY_WINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QListWidget>
#include <QLabel>
#include <QWidget>
#include <QFrame>
#include <QComboBox>
#include <QStackedWidget>
#include "lobby_client.h"
#include "waiting_room_widget.h"
#include "create_game_dialog.h"
#include "car_selection_dialog.h"
#include "game_card_widget.h"

class LobbyWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit LobbyWindow(QWidget *parent = nullptr);
    ~LobbyWindow();

private slots:
    void onCreateGameClicked();
    void onJoinGameClicked();
    void onRefreshClicked();
    void onGameSelected(QListWidgetItem *item);
    void onGameDoubleClicked(QListWidgetItem *item);
    void onThemeChanged(int index);
    void applyTheme();
    
    // Navegación entre páginas
    void showLobbyPage();
    void showCreateGamePage();
    void showCarSelectionPage();
    
    // Acciones del formulario de crear partida
    void onCreateGameSubmit();
    void onCreateGameCancel();
    
    // Acciones de selección de auto
    void onCarSelectionConfirm();
    void onCarSelectionCancel();
    
    // Acciones de sala de espera (delegadas al widget)
    void onWaitingRoomLeaveRequested();
    void onWaitingRoomReadyChanged(bool ready);
    void onWaitingRoomStartGame();
    
    // Slots para respuestas del cliente
    void onConnected();
    void onGamesListReceived(std::vector<GameInfo> games);
    void onGameCreated(int gameId);
    void onGameJoined(int gameId);
    void onPlayersListUpdated(std::vector<PlayerInfo> players);
    void onGameStarting();
    void onError(QString message);

private:
    void setupUI();
    void updateGamesList(const std::vector<GameInfo>& games);
    QString getCarEmoji(const QString& carType) const;
    
    // Métodos para crear cada página
    QWidget* createLobbyPage();

    // Cliente de red (interfaz abstracta)
    ILobbyClient* lobbyClient;
    
    // Cache de partidas
    std::vector<GameInfo> currentGames;
    
    // Stack de páginas
    QStackedWidget* stackedWidget;
    QWidget* lobbyPage;
    CreateGameDialog* createGameWidget;
    CarSelectionDialog* carSelectionWidget;
    WaitingRoomWidget* waitingRoomWidget;  
    
    // Widgets del lobby
    QListWidget *gamesListWidget;
    QPushButton *createGameButton;
    QPushButton *joinGameButton;
    QPushButton *refreshButton;
    QLabel *titleLabel;
    QLabel *statusLabel;
    QComboBox *themeSelector;
    
    // Estado de la aplicación
    enum class CarSelectionMode {
        Creating,  // Creando nueva partida
        Joining    // Uniéndose a partida existente
    };
    CarSelectionMode carSelectionMode;
    int joiningGameId;
    int currentGameId;
    
};

#endif // LOBBY_WINDOW_H
