#pragma once

#include <QLabel>
#include <QListWidget>
#include <QPushButton>
#include <QWidget>
#include <vector>

#include "client/connexion/connexion.h"

class WaitingWindow final : public QWidget {
    Q_OBJECT

    Connexion& connexion;

   public:
    explicit WaitingWindow(QWidget* parent, Connexion&);
    ~WaitingWindow() override;

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
