#pragma once

#include <QLabel>
#include <QProgressBar>

#include "client/qt/lobby_client.h"
#include "common/structs.h"

class GameCardWidget : public QFrame {
    Q_OBJECT

   public:
    explicit GameCardWidget(const SessionInfo& info, QWidget* parent = nullptr);

    void updateInfo(const SessionInfo& info);
    void applyTheme();

   private:
    void setupUI();
    QString getMapIcon(const QString& mapName) const;
    QString getStatusText(const SessionStatus& status) const;
    QString getStatusColor(const SessionStatus& status) const;

    SessionInfo info;

    // Widgets que necesitan actualización dinámica
    QLabel* mapIconLabel;
    QLabel* nameLabel;
    QLabel* mapLabel;
    QLabel* playersLabel;
    QProgressBar* playersBar;
    QLabel* statusLabel;
};
