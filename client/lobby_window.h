#ifndef LOBBY_WINDOW_H
#define LOBBY_WINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QListWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QWidget>
#include <QFrame>
#include <QComboBox>
#include "lobby_client.h"

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
    void onGameDoubleClicked(QListWidgetItem *item);  // Nuevo: doble click
    void onThemeChanged(int index);  // Cambio de tema desde el selector
    void applyTheme();  // Nuevo: aplicar tema a todos los widgets
    
    // Slots para respuestas del cliente
    void onConnected();
    void onGamesListReceived(std::vector<GameInfo> games);
    void onGameCreated(int gameId);
    void onGameJoined(int gameId);
    void onError(QString message);

private:
    void setupUI();
    void updateGamesList(const std::vector<GameInfo>& games);
    QString getMapIcon(const QString& mapName) const;
    QString getCarEmoji(const QString& carType) const;
    QString getStatusText(const QString& status) const;
    QString getStatusColor(const QString& status) const;
    QWidget* createGameItemWidget(const GameInfo& game);

    // Cliente de red (interfaz abstracta)
    ILobbyClient* lobbyClient;
    
    // Cache de partidas
    std::vector<GameInfo> currentGames;
    
    // Widgets
    QWidget *centralWidget;
    QListWidget *gamesListWidget;
    QPushButton *createGameButton;
    QPushButton *joinGameButton;
    QPushButton *refreshButton;
    QLabel *titleLabel;
    QLabel *statusLabel;
    QComboBox *themeSelector;  // Nuevo: Selector de temas
};

#endif // LOBBY_WINDOW_H
