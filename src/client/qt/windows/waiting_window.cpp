#include "client/qt/windows/waiting_window.h"

#include <QFrame>
#include <QHBoxLayout>
#include <QMessageBox>
#include <vector>

#include "client/qt/theme_manager.h"
#include "client/qt/ui/CarSprite.h"
#include "spdlog/spdlog.h"

WaitingWindow::WaitingWindow(QWidget* parent, Connexion& connexion)
    : QWidget(parent),
      api(connexion.get_api()),
      id(connexion.unique_id),
      currentGameId(-1),
      playerIsReady(false) {
    setupUI();
    Responder::subscribe(connexion);
}
WaitingWindow::~WaitingWindow() { Responder::unsubscribe(); }
void WaitingWindow::on_session_snapshot(
    const SessionConfig&, const std::vector<PlayerInfo>& player_infos) {
    QMetaObject::invokeMethod(
        this,
        [this, player_infos]() {
            updatePlayersList(player_infos);
            for (const auto& player : player_infos) {
                if (player.id == id) {
                    updateReadyButton(player.isReady);
                    break;
                }
            }
        },
        Qt::QueuedConnection);
}
void WaitingWindow::on_start_game() {
    QMetaObject::invokeMethod(
        this, [this]() { emit startGameRequested(); }, Qt::QueuedConnection);
}

void WaitingWindow::on_leave_response() {
    QMetaObject::invokeMethod(
        this, [this]() { emit leaveGameRequested(); }, Qt::QueuedConnection);
}

void WaitingWindow::setupUI() {
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(50, 30, 50, 30);

    // Título
    titleLabel = new QLabel("🎮 Sala de Espera", this);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(24);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    titleLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(titleLabel);

    // Estado
    statusLabel =
        new QLabel("Esperando que todos los jugadores estén listos...", this);
    statusLabel->setAlignment(Qt::AlignCenter);
    QFont statusFont = statusLabel->font();
    statusFont.setPointSize(12);
    statusLabel->setFont(statusFont);
    layout->addWidget(statusLabel);

    layout->addSpacing(20);

    // Etiqueta de lista
    QLabel* playersLabel = new QLabel("👥 Jugadores en la Sala:", this);
    QFont playersLabelFont = playersLabel->font();
    playersLabelFont.setPointSize(14);
    playersLabelFont.setBold(true);
    playersLabel->setFont(playersLabelFont);
    layout->addWidget(playersLabel);

    // Lista de jugadores
    playersListWidget = new QListWidget(this);
    playersListWidget->setMinimumHeight(300);
    layout->addWidget(playersListWidget);

    layout->addStretch();

    // Botones
    QHBoxLayout* buttonLayout = new QHBoxLayout();

    leaveButton = new QPushButton("🚪 Salir de la Partida", this);
    leaveButton->setMinimumHeight(45);
    connect(leaveButton, &QPushButton::clicked, this,
            &WaitingWindow::onLeaveClicked);
    buttonLayout->addWidget(leaveButton);

    readyButton = new QPushButton("✅ Listo", this);
    readyButton->setMinimumHeight(45);
    readyButton->setCheckable(true);
    readyButton->setStyleSheet(
        "background-color: #27ae60; color: white; font-weight: bold;");
    connect(readyButton, &QPushButton::toggled, this,
            &WaitingWindow::onReadyToggled);
    buttonLayout->addWidget(readyButton);

    layout->addLayout(buttonLayout);
}

void WaitingWindow::setGameInfo(int gameId,
                                const std::vector<PlayerInfo>& players) {
    currentGameId = gameId;
    currentPlayers = players;
    playerIsReady = false;

    // Reset UI
    readyButton->setChecked(false);
    readyButton->setText("✅ Listo");
    readyButton->setStyleSheet(
        "background-color: #27ae60; color: white; font-weight: bold;");

    updatePlayersList(players);
}

void WaitingWindow::updatePlayersList(const std::vector<PlayerInfo>& players) {
    currentPlayers = players;
    playersListWidget->clear();

    ThemeManager& theme = ThemeManager::instance();
    const ColorPalette& palette = theme.getCurrentPalette();

    for (const PlayerInfo& player : players) {
        QListWidgetItem* item = new QListWidgetItem(playersListWidget);

        // Frame del jugador
        QFrame* frame = new QFrame();
        frame->setFrameShape(QFrame::Box);
        frame->setStyleSheet(theme.gameCardStyle());

        QHBoxLayout* hlayout = new QHBoxLayout(frame);
        hlayout->setContentsMargins(15, 10, 15, 10);

        // Icono del auto
        QLabel* carIcon =
            new QLabel(CarSprite::getSprite(player.carType).c_str());
        carIcon->setStyleSheet(
            QString("font-size: 32px; border: none; color: %1;")
                .arg(palette.textPrimary));
        hlayout->addWidget(carIcon);

        // Nombre
        QLabel* nameLabel = new QLabel(player.name.c_str());
        nameLabel->setStyleSheet(
            QString(
                "font-size: 16px; font-weight: bold; border: none; color: %1;")
                .arg(palette.textPrimary));
        hlayout->addWidget(nameLabel);

        // Badge de host
        if (player.isHost) {
            QLabel* hostBadge = new QLabel("👑 Host");
            hostBadge->setStyleSheet(
                "font-size: 12px;"
                "color: gold;"
                "background-color: rgba(255, 215, 0, 0.2);"
                "border: 1px solid gold;"
                "border-radius: 10px;"
                "padding: 3px 10px;");
            hlayout->addWidget(hostBadge);
        }

        hlayout->addStretch();

        // Estado ready
        statusLabel = new QLabel(player.isReady ? "✅ Listo" : "⏳ Esperando");
        statusLabel->setStyleSheet(
            QString("font-size: 14px; font-weight: bold; color: %1; "
                    "border: none; padding: 5px 15px;")
                .arg(player.isReady ? "#27ae60" : "#95a5a6"));
        hlayout->addWidget(statusLabel);

        item->setSizeHint(frame->sizeHint());
        playersListWidget->addItem(item);
        playersListWidget->setItemWidget(item, frame);
    }

    // Actualizar título con contador
    titleLabel->setText(
        QString("🎮 Sala de Espera (%1/%2)").arg(players.size()).arg(8));

    updateStatusMessage();
}

void WaitingWindow::updateStatusMessage() {
    // Verificar si todos están listos
    bool allReady = true;
    for (const auto& p : currentPlayers) {
        if (!p.isReady) {
            allReady = false;
        }
    }

    if (allReady && currentPlayers.size() >= 2) {
        statusLabel->setText("🏁 ¡Todos listos! La carrera comenzará pronto...");
        statusLabel->setStyleSheet(
            QString("color: #27ae60; font-size: 14px; ") +
            "font-weight: bold;");

        // Emitir señal para que el padre inicie el juego
        // emit startGameRequested();
    } else {
        statusLabel->setText(
            "⏳ Esperando que todos los jugadores estén listos...");
        statusLabel->setStyleSheet("color: #95a5a6; font-size: 12px;");
    }
}
void WaitingWindow::updateReadyButton(bool checked) {
    playerIsReady = checked;

    if (checked) {
        readyButton->setText("❌ Cancelar");
        readyButton->setStyleSheet(
            "background-color: #e74c3c; color: white; font-weight: bold;");
    } else {
        readyButton->setText("✅ Listo");
        readyButton->setStyleSheet(
            "background-color: #27ae60; color: white; font-weight: bold;");
    }
}

void WaitingWindow::onLeaveClicked() {
    QMessageBox::StandardButton reply =
        QMessageBox::question(this, "Salir de la Partida",
                              "¿Estás seguro que quieres salir de la partida?",
                              QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        api.request_leave_current_session();
    }
}

void WaitingWindow::onReadyToggled(bool checked) {
    // emit readyStateChanged(checked);
    api.set_ready(checked);
}
