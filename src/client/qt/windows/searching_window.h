#pragma once

#include <client/connexion/connexion.h>

#include <QComboBox>
#include <QListWidget>
#include <QPushButton>
#include <QWidget>
#include <string>
#include <unordered_map>
#include <vector>

#include "client/qt/game_card_widget.h"
#include "common/structs.h"

class SearchingWindow final : public QWidget, Connexion::Responder {
    Q_OBJECT

    Api &api;

   public:
    explicit SearchingWindow(QWidget *parent, Connexion &);
    ~SearchingWindow() override;

    // Connexion overrides
    void on_search_response(const std::vector<SessionInfo> &) override;
    void on_join_response(const SessionInfo &,
                          const std::vector<CarStaticInfo> &) override;

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

    void onError(QString message);

   private:
    void updateGamesList(const std::vector<SessionInfo> &games);
    QString getCarEmoji(const QString &carType) const;

    void showEvent(QShowEvent *event) override;

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
