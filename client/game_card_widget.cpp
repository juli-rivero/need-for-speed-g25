#include "game_card_widget.h"
#include "theme_manager.h"
#include <QHBoxLayout>
#include <QVBoxLayout>

GameCardWidget::GameCardWidget(const GameInfo& info, QWidget* parent)
    : QFrame(parent), gameInfo(info) {
    
    setupUI();
    
    // Conectar al cambio de tema
    connect(&ThemeManager::instance(), &ThemeManager::themeChanged,
            this, &GameCardWidget::applyTheme);
    
    applyTheme();
}

void GameCardWidget::setupUI() {
    setFrameShape(QFrame::Box);
    
    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setContentsMargins(10, 10, 10, 10);
    
    // === Columna 1: Icono del mapa ===
    mapIconLabel = new QLabel(getMapIcon(gameInfo.map));
    mapIconLabel->setStyleSheet("font-size: 32px; border: none;");
    mapIconLabel->setFixedWidth(50);
    mapIconLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(mapIconLabel);
    
    // === Columna 2: Información principal ===
    QVBoxLayout* infoLayout = new QVBoxLayout();
    
    nameLabel = new QLabel(gameInfo.name);
    nameLabel->setStyleSheet("font-size: 16px; font-weight: bold; border: none;");
    infoLayout->addWidget(nameLabel);
    
    mapLabel = new QLabel(QString("Mapa: %1").arg(gameInfo.map));
    mapLabel->setStyleSheet("font-size: 12px; border: none;");
    infoLayout->addWidget(mapLabel);
    
    layout->addLayout(infoLayout, 1);
    
    // === Columna 3: Jugadores con barra ===
    QVBoxLayout* playersLayout = new QVBoxLayout();
    
    playersLabel = new QLabel(QString("👥 %1/%2").arg(gameInfo.currentPlayers).arg(gameInfo.maxPlayers));
    playersLabel->setStyleSheet("font-size: 14px; font-weight: bold; border: none;");
    playersLayout->addWidget(playersLabel);
    
    playersBar = new QProgressBar();
    playersBar->setRange(0, gameInfo.maxPlayers);
    playersBar->setValue(gameInfo.currentPlayers);
    playersBar->setTextVisible(false);
    playersBar->setMaximumHeight(8);
    playersLayout->addWidget(playersBar);
    
    layout->addLayout(playersLayout);
    
    // === Columna 4: Estado ===
    QVBoxLayout* statusLayout = new QVBoxLayout();
    
    statusLabel = new QLabel(getStatusText(gameInfo.status));
    statusLabel->setStyleSheet(QString(
        "font-size: 12px;"
        "font-weight: bold;"
        "color: white;"
        "background-color: %1;"
        "border: none;"
        "border-radius: 10px;"
        "padding: 5px 15px;"
    ).arg(getStatusColor(gameInfo.status)));
    statusLabel->setAlignment(Qt::AlignCenter);
    statusLabel->setFixedWidth(100);
    
    statusLayout->addWidget(statusLabel);
    statusLayout->addStretch();
    
    layout->addLayout(statusLayout);
}

void GameCardWidget::updateInfo(const GameInfo& info) {
    gameInfo = info;
    
    // Actualizar widgets
    mapIconLabel->setText(getMapIcon(gameInfo.map));
    nameLabel->setText(gameInfo.name);
    mapLabel->setText(QString("Mapa: %1").arg(gameInfo.map));
    playersLabel->setText(QString("👥 %1/%2").arg(gameInfo.currentPlayers).arg(gameInfo.maxPlayers));
    
    playersBar->setRange(0, gameInfo.maxPlayers);
    playersBar->setValue(gameInfo.currentPlayers);
    
    statusLabel->setText(getStatusText(gameInfo.status));
    statusLabel->setStyleSheet(QString(
        "font-size: 12px;"
        "font-weight: bold;"
        "color: white;"
        "background-color: %1;"
        "border: none;"
        "border-radius: 10px;"
        "padding: 5px 15px;"
    ).arg(getStatusColor(gameInfo.status)));
}

void GameCardWidget::applyTheme() {
    ThemeManager& theme = ThemeManager::instance();
    const ColorPalette& palette = theme.getCurrentPalette();
    
    // Aplicar estilo del frame
    setStyleSheet(theme.gameCardStyle());
    
    // Actualizar colores de texto
    mapIconLabel->setStyleSheet(QString("font-size: 32px; border: none; color: %1;").arg(palette.textPrimary));
    nameLabel->setStyleSheet(QString("font-size: 16px; font-weight: bold; border: none; color: %1;").arg(palette.textPrimary));
    mapLabel->setStyleSheet(QString("font-size: 12px; border: none; color: %1;").arg(palette.textSecondary));
    playersLabel->setStyleSheet(QString("font-size: 14px; font-weight: bold; border: none; color: %1;").arg(palette.textPrimary));
    
    // Actualizar barra de progreso
    playersBar->setStyleSheet(theme.progressBarStyle());
}

QString GameCardWidget::getMapIcon(const QString& mapName) const {
    if (mapName == "city") return "🏙️";
    return "🏎️";
}

QString GameCardWidget::getStatusText(const QString& status) const {
    if (status == "waiting") return "Esperando";
    if (status == "ready") return "Lista";
    if (status == "playing") return "En juego";
    if (status == "full") return "Llena";
    if (status == "finished") return "Finalizada";
    return "Desconocido";
}

QString GameCardWidget::getStatusColor(const QString& status) const {
    if (status == "waiting") return "#4CAF50";
    if (status == "ready") return "#FF9800";
    if (status == "playing") return "#F44336";
    if (status == "full") return "#9E9E9E";
    if (status == "finished") return "#607D8B";
    return "#000000";
}
