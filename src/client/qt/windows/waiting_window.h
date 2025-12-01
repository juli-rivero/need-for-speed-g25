#pragma once

#include <QLabel>
#include <QListWidget>
#include <QPushButton>
#include <QWidget>
#include <string>
#include <vector>

#include "client/connexion/connexion.h"
#include "common/structs.h"

class WaitingWindow final : public QWidget, Connexion::Responder {
    Q_OBJECT

    Api& api;
    PlayerId id;

   public:
    explicit WaitingWindow(QWidget* parent, Connexion&);
    ~WaitingWindow() override;

    void on_session_snapshot(const SessionConfig&,
                             const std::vector<PlayerInfo>&) override;
    void on_start_game(const CityInfo&, const RaceInfo&,
                       const std::vector<UpgradeChoice>&) override;
    void on_leave_response() override;

    // Configurar la sala de espera
    void setGameInfo(int gameId, const std::vector<PlayerInfo>& players);
    void updatePlayersList(const std::vector<PlayerInfo>& players);

    // Estado
    bool isPlayerReady() const { return playerIsReady; }
    int getGameId() const { return currentGameId; }

   signals:
    void leaveGameRequested();
    // void readyStateChanged(bool ready);
    void startGameRequested(const CityInfo&, const RaceInfo&,
                            const std::vector<UpgradeChoice>&);

   private slots:
    void onLeaveClicked();
    void onReadyToggled(bool checked);

   private:
    void setupUI();
    void updateStatusMessage();
    void updateReadyButton(bool checked);

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
