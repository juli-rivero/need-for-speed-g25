#ifndef WAITING_ROOM_WIDGET_H
#define WAITING_ROOM_WIDGET_H

#include <QWidget>
#include <QLabel>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <vector>
#include "lobby_client.h"

class WaitingRoomWidget : public QWidget {
    Q_OBJECT

public:
    explicit WaitingRoomWidget(QWidget* parent = nullptr);
    ~WaitingRoomWidget() = default;

    // Configurar la sala de espera
    void setGameInfo(int gameId, const std::vector<PlayerInfo>& players);
    void updatePlayersList(const std::vector<PlayerInfo>& players);
    
    // Estado
    bool isPlayerReady() const { return playerIsReady; }
    int getGameId() const { return currentGameId; }

signals:
    void leaveGameRequested();
    void readyStateChanged(bool ready);
    void startGameRequested();

private slots:
    void onLeaveClicked();
    void onReadyToggled(bool checked);

private:
    void setupUI();
    void updateStatusMessage();
    QString getCarEmoji(int carType) const;

    // Widgets
    QLabel* titleLabel;
    QLabel* statusLabel;
    QListWidget* playersListWidget;
    QPushButton* leaveButton;
    QPushButton* readyButton;

    // Estado
    int currentGameId;
    bool playerIsReady;
    std::vector<PlayerInfo> currentPlayers;
};

#endif // WAITING_ROOM_WIDGET_H
