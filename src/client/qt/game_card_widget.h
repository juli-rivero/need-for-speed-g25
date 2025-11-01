#ifndef GAME_CARD_WIDGET_H
#define GAME_CARD_WIDGET_H

#include <QFrame>
#include <QLabel>
#include <QProgressBar>
#include "lobby_client.h"

class GameCardWidget : public QFrame {
    Q_OBJECT

public:
    explicit GameCardWidget(const GameInfo& info, QWidget* parent = nullptr);
    
    void updateInfo(const GameInfo& info);
    void applyTheme();
    
    int getGameId() const { return gameInfo.id; }

private:
    void setupUI();
    QString getMapIcon(const QString& mapName) const;
    QString getStatusText(const QString& status) const;
    QString getStatusColor(const QString& status) const;

    GameInfo gameInfo;
    
    // Widgets que necesitan actualización dinámica
    QLabel* mapIconLabel;
    QLabel* nameLabel;
    QLabel* mapLabel;
    QLabel* playersLabel;
    QProgressBar* playersBar;
    QLabel* statusLabel;
};

#endif // GAME_CARD_WIDGET_H
