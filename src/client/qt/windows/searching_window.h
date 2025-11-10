#pragma once

#include <client/connexion/connexion.h>

#include <QComboBox>
#include <QListWidget>
#include <QPushButton>
#include <QWidget>
#include <string>
#include <vector>

#include "client/qt/game_card_widget.h"
#include "common/structs.h"

class SearchingWindow final : public QWidget, Connexion::Responder {
    Q_OBJECT

    Api &api;

   public:
    explicit SearchingWindow(QWidget *parent, Connexion &);

    // Connexion overrides
    void on_search_response(const std::vector<SessionInfo> &) override;
    void on_join_response(const SessionInfo &) override;

   signals:
    void createGameClicked();
    void joinGameClicked();

   private slots:
    void onCreateGameClicked();
    void onJoinGameClicked();
    void onRefreshClicked();
    void onGameSelected(QListWidgetItem *item);
    void onGameDoubleClicked(QListWidgetItem *item);
    void onThemeChanged(int index);
    void applyTheme();

    // Slots para respuestas del cliente
    // void onConnected();

    // void onGameCreated(int gameId); Se va a hacer en create_game_dialog
    // void onPlayersListUpdated(std::vector<PlayerInfo> players); se va a hacer
    // en waiting_room_widget void onGameStarting(); se va a hacer en
    // waiting_room_widget
    void onError(QString message);

   private:
    void updateGamesList(const std::vector<SessionInfo> &games);
    QString getCarEmoji(const QString &carType) const;

    // Métodos para crear cada página
    void createThis();

    // Cache de partidas
    std::vector<SessionInfo> currentGames;

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
