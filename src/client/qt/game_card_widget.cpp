#include "client/qt/game_card_widget.h"

#include <QVBoxLayout>

#include "client/qt/theme_manager.h"
#include "common/structs.h"

GameCardWidget::GameCardWidget(const SessionInfo& info, QWidget* parent)
    : QFrame(parent), info(info) {
    setupUI();

    // Conectar al cambio de tema
    connect(&ThemeManager::instance(), &ThemeManager::themeChanged, this,
            &GameCardWidget::applyTheme);

    applyTheme();
}

void GameCardWidget::setupUI() {
    setFrameShape(QFrame::Box);

    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setContentsMargins(10, 10, 10, 10);

    // === Columna 1: Icono del mapa ===
    mapIconLabel = new QLabel(getMapIcon(info.city.c_str()));
    mapIconLabel->setStyleSheet("font-size: 32px; border: none;");
    mapIconLabel->setFixedWidth(50);
    mapIconLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(mapIconLabel);

    // === Columna 2: Información principal ===
    QVBoxLayout* infoLayout = new QVBoxLayout();

    nameLabel = new QLabel(info.city.c_str());
    nameLabel->setStyleSheet(
        "font-size: 16px; font-weight: bold; border: none;");
    infoLayout->addWidget(nameLabel);

    mapLabel = new QLabel(QString("Mapa: %1").arg(info.city.c_str()));
    mapLabel->setStyleSheet("font-size: 12px; border: none;");
    infoLayout->addWidget(mapLabel);

    layout->addLayout(infoLayout, 1);

    // === Columna 3: Jugadores con barra ===
    QVBoxLayout* playersLayout = new QVBoxLayout();

    playersLabel = new QLabel(
        QString("👥 %1/%2").arg(info.currentPlayers).arg(info.maxPlayers));
    playersLabel->setStyleSheet(
        "font-size: 14px; font-weight: bold; border: none;");
    playersLayout->addWidget(playersLabel);

    playersBar = new QProgressBar();
    playersBar->setRange(0, info.maxPlayers);
    playersBar->setValue(info.currentPlayers);
    playersBar->setTextVisible(false);
    playersBar->setMaximumHeight(8);
    playersLayout->addWidget(playersBar);

    layout->addLayout(playersLayout);

    // === Columna 4: Estado ===
    QVBoxLayout* statusLayout = new QVBoxLayout();

    statusLabel = new QLabel(getStatusText(info.status));
    statusLabel->setStyleSheet(QString("font-size: 12px;"
                                       "font-weight: bold;"
                                       "color: white;"
                                       "background-color: %1;"
                                       "border: none;"
                                       "border-radius: 10px;"
                                       "padding: 5px 15px;")
                                   .arg(getStatusColor(info.status)));
    statusLabel->setAlignment(Qt::AlignCenter);
    statusLabel->setFixedWidth(100);

    statusLayout->addWidget(statusLabel);
    statusLayout->addStretch();

    layout->addLayout(statusLayout);
}

void GameCardWidget::updateInfo(const SessionInfo& new_info) {
    info = new_info;

    // Actualizar widgets
    mapIconLabel->setText(getMapIcon(info.city.c_str()));
    nameLabel->setText(info.name.c_str());
    mapLabel->setText(QString("Mapa: %1").arg(info.city.c_str()));
    playersLabel->setText(
        QString("👥 %1/%2").arg(info.currentPlayers).arg(info.maxPlayers));

    playersBar->setRange(0, info.maxPlayers);
    playersBar->setValue(info.currentPlayers);

    statusLabel->setText(getStatusText(info.status));
    statusLabel->setStyleSheet(QString("font-size: 12px;"
                                       "font-weight: bold;"
                                       "color: white;"
                                       "background-color: %1;"
                                       "border: none;"
                                       "border-radius: 10px;"
                                       "padding: 5px 15px;")
                                   .arg(getStatusColor(info.status)));
}

void GameCardWidget::applyTheme() {
    ThemeManager& theme = ThemeManager::instance();
    const ColorPalette& palette = theme.getCurrentPalette();

    // Aplicar estilo del frame
    setStyleSheet(theme.gameCardStyle());

    // Actualizar colores de texto
    mapIconLabel->setStyleSheet(
        QString("font-size: 32px; border: none; color: %1;")
            .arg(palette.textPrimary));
    nameLabel->setStyleSheet(
        QString("font-size: 16px; font-weight: bold; border: none; color: %1;")
            .arg(palette.textPrimary));
    mapLabel->setStyleSheet(QString("font-size: 12px; border: none; color: %1;")
                                .arg(palette.textSecondary));
    playersLabel->setStyleSheet(
        QString("font-size: 14px; font-weight: bold; border: none; color: %1;")
            .arg(palette.textPrimary));

    // Actualizar barra de progreso
    playersBar->setStyleSheet(theme.progressBarStyle());
}

QString GameCardWidget::getMapIcon(const QString& mapName) const {
    if (mapName == "city") return "🏙️";
    return "🏎️";
}

QString GameCardWidget::getStatusText(const SessionStatus& status) const {
    switch (status) {
        case SessionStatus::Full:
            return "Llena";
        case SessionStatus::Playing:
            return "En juego";
        case SessionStatus::Waiting:
            return "Esperando";
    }
    /*if (status == "waiting") return "Esperando";
    if (status == "ready") return "Lista";
    if (status == "playing") return "En juego";
    if (status == "full") return "Llena";
    if (status == "finished") return "Finalizada";*/
    return "Desconocido";
}

QString GameCardWidget::getStatusColor(const SessionStatus& status) const {
    switch (status) {
        case SessionStatus::Full:
            return "#9E9E9E";
        case SessionStatus::Playing:
            return "#F44336";
        case SessionStatus::Waiting:
            return "#4CAF50";
    }
    /*if (status == "waiting") return "#4CAF50";
    if (status == "ready") return "#FF9800";
    if (status == "playing") return "#F44336";
    if (status == "full") return "#9E9E9E";
    if (status == "finished") return "#607D8B";*/
    return "#000000";
}
