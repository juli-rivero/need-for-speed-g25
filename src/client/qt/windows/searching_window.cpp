#include "client/qt/windows/searching_window.h"

#include <QFormLayout>
#include <QFrame>
#include <QMessageBox>
#include <QToolBar>
#include <string>
#include <vector>

#include "client/qt/game_card_widget.h"
#include "client/qt/theme_manager.h"
#include "spdlog/spdlog.h"

SearchingWindow::SearchingWindow(QWidget* parent, Connexion& connexion)
    : QWidget(parent), api(connexion.get_api()), currentGameId(-1) {
    spdlog::trace("creating lobby window");

    // Conectar el cambio de tema global
    connect(&ThemeManager::instance(), &ThemeManager::themeChanged, this,
            &SearchingWindow::applyTheme);

    createThis();
    // Aplicar tema inicial
    applyTheme();

    Responder::subscribe(connexion);
}
SearchingWindow::~SearchingWindow() { Responder::unsubscribe(); }

void SearchingWindow::on_search_response(
    const std::vector<SessionInfo>& session_infos) {
    QMetaObject::invokeMethod(
        this, [this, session_infos]() { updateGamesList(session_infos); },
        Qt::QueuedConnection);
}

void SearchingWindow::updateGamesList(const std::vector<SessionInfo>& games) {
    currentGames = games;

    // Limpiar lista actual
    gamesListWidget->clear();

    // Si no hay partidas, mostrar mensaje
    if (games.empty()) {
        QListWidgetItem* emptyItem =
            new QListWidgetItem("No hay partidas disponibles");
        emptyItem->setFlags(Qt::ItemIsEnabled);  // No seleccionable
        emptyItem->setForeground(QBrush(QColor("#ff0033")));
        gamesListWidget->addItem(emptyItem);
        return;
    }

    // Agregar cada partida usando GameCardWidget
    for (const SessionInfo& game : games) {
        QListWidgetItem* item = new QListWidgetItem(gamesListWidget);
        item->setData(Qt::UserRole,
                      game.name.c_str());  // Guardar ID de la partida

        GameCardWidget* gameCard = new GameCardWidget(game);
        item->setSizeHint(gameCard->sizeHint());

        gamesListWidget->addItem(item);
        gamesListWidget->setItemWidget(item, gameCard);
    }
}

// NAVEGACIÓN ENTRE PÁGINAS

void SearchingWindow::onJoinGameClicked() {
    QListWidgetItem* selectedItem = gamesListWidget->currentItem();
    if (selectedItem) {
        std::string gameId =
            selectedItem->data(Qt::UserRole).toString().toUtf8().constData();

        // Verificar que no esté llena
        auto it = std::find_if(
            currentGames.begin(), currentGames.end(),
            [gameId](const SessionInfo& g) { return g.name == gameId; });

        if (it != currentGames.end() && it->status == SessionStatus::Full) {
            QMessageBox::warning(this, "Partida Llena",
                                 "Esta partida ya está llena. Elige otra.");
            return;
        }

        api.request_join_session(gameId);

        statusLabel->setText("Uniéndose a la partida...");
        statusLabel->setStyleSheet("color: orange;");
    }
}

void SearchingWindow::onRefreshClicked() {
    statusLabel->setText("Actualizando...");
    statusLabel->setStyleSheet("color: orange;");

    // Solicitar lista actualizada al servidor
    api.request_search_all_sessions();
}

void SearchingWindow::onGameSelected(QListWidgetItem* item) {
    // Habilitar botón de unirse cuando se selecciona una partida
    joinGameButton->setEnabled(item != nullptr);
}

void SearchingWindow::onGameDoubleClicked(QListWidgetItem* item) {
    if (!item) return;

    std::string gameId =
        item->data(Qt::UserRole).toString().toUtf8().constData();

    // Verificar que no esté llena o en juego
    auto it = std::find_if(
        currentGames.begin(), currentGames.end(),
        [gameId](const SessionInfo& g) { return g.name == gameId; });

    if (it != currentGames.end()) {
        if (it->status == SessionStatus::Full) {
            QMessageBox::warning(this, "Partida Llena",
                                 "Esta partida ya está llena. Elige otra.");
            return;
        }
        if (it->status == SessionStatus::Playing) {
            QMessageBox::warning(this, "Partida en Curso",
                                 "Esta partida ya comenzó. No puedes unirte.");
            return;
        }
    }
    api.request_join_session(gameId);

    // Unirse directamente con doble click
    statusLabel->setText("Uniéndose a la partida...");
    statusLabel->setStyleSheet("color: orange;");
}

void SearchingWindow::on_join_response(const SessionInfo&,
                                       const std::vector<CarStaticInfo>&) {

    spdlog::trace("unido a partida");

    QMetaObject::invokeMethod(
        this, [this]() { emit joinGameClicked(); }, Qt::QueuedConnection);

}

void SearchingWindow::onError(QString message) {
    statusLabel->setText("❌ Error: " + message);
    statusLabel->setStyleSheet("color: red;");

    QMessageBox::critical(this, "Error", message);
}

void SearchingWindow::showEvent(QShowEvent* event) {
    api.request_search_all_sessions();
    QWidget::showEvent(event);
}

// GESTIÓN DE TEMAS DINÁMICOS

void SearchingWindow::onThemeChanged(int index) {
    if (index < 0 || !themeSelector) return;

    // Obtener el tipo de tema del combo box
    ThemeType selectedTheme =
        static_cast<ThemeType>(themeSelector->itemData(index).toInt());

    // Actualizar el ThemeManager (esto disparará la señal themeChanged)
    ThemeManager::instance().setTheme(selectedTheme);
}

void SearchingWindow::applyTheme() {
    ThemeManager& theme = ThemeManager::instance();
    const ColorPalette& palette = theme.getCurrentPalette();

    setStyleSheet(QString(
        "QWidget {"
        "    background-color: %1;"
        "}"
        "QLabel {"
        "    color: %2;"
        "    background-color: transparent;"
        "}")
        .arg(palette.cardBackgroundHover)
        .arg(palette.textPrimary));

    // Botones
    if (createGameButton) createGameButton->setStyleSheet(theme.buttonPrimaryStyle());
    if (joinGameButton) joinGameButton->setStyleSheet(theme.buttonSecondaryStyle());
    if (refreshButton) refreshButton->setStyleSheet(theme.buttonActionStyle());

    // Título
    if (titleLabel) {
        titleLabel->setStyleSheet(QString(
            "color: %1; font-size: 18px; font-weight: bold; padding: 8px;")
            .arg(palette.textPrimary));
    }

    // Status label con mejor contraste
    if (statusLabel) {
        QString text = statusLabel->text();
        if (text.contains("❌") || text.toLower().contains("error")) {
            statusLabel->setStyleSheet(
                "color: white; background-color: #e74c3c; padding: 6px 10px; border-radius: 4px;");
        } else if (text.contains("✅") || text.contains("Conectado")) {
            statusLabel->setStyleSheet(
                "color: white; background-color: #27ae60; padding: 6px 10px; border-radius: 4px;");
        } else {
            statusLabel->setStyleSheet(
                QString("color: %1; background-color: %2; padding: 6px 10px; border-radius: 4px;")
                .arg(palette.textPrimary).arg(palette.cardBackground));
        }
    }

    // Selector de temas mejorado
    if (themeSelector) {
        themeSelector->setStyleSheet(
            QString("QComboBox {"
                    "    background-color: %1;"
                    "    color: %2;"
                    "    border: 2px solid %3;"
                    "    border-radius: 4px;"
                    "    padding: 6px;"
                    "    min-width: 180px;"
                    "}"
                    "QComboBox:hover {"
                    "    border-color: %4;"
                    "}"
                    "QComboBox QAbstractItemView {"
                    "    background-color: %1;"
                    "    color: %2;"
                    "    selection-background-color: %4;"
                    "}")
                .arg(palette.cardBackground)
                .arg(palette.textPrimary)
                .arg(palette.primaryColor)
                .arg(palette.secondaryColor));
    }

    // Lista de juegos
    if (gamesListWidget) {
        gamesListWidget->setStyleSheet(
            QString("QListWidget {"
                    "    background-color: %1;"
                    "    border: 1px solid %2;"
                    "    border-radius: 5px;"
                    "}"
                    "QListWidget::item {"
                    "    border-bottom: 1px solid %2;"
                    "}"
                    "QListWidget::item:selected {"
                    "    background-color: %3;"
                    "}")
                .arg(palette.cardBackground)
                .arg(palette.borderColor)
                .arg(palette.primaryColor));
    }

    updateGamesList(currentGames);
}

// ========== MÉTODOS PARA CREAR PÁGINAS ==========

void SearchingWindow::createThis() {
    QVBoxLayout* layout = new QVBoxLayout(this);

    // ===== Toolbar con selector de tema =====
    QHBoxLayout* toolbarLayout = new QHBoxLayout();

    QLabel* themeLabel = new QLabel("🎨 Tema:", this);
    toolbarLayout->addWidget(themeLabel);

    themeSelector = new QComboBox(this);
    themeSelector->addItem("🎨 Default", static_cast<int>(ThemeType::Default));
    themeSelector->addItem("🌙 Dark Mode",
                           static_cast<int>(ThemeType::DarkMode));
    themeSelector->addItem("🏎️ Racing Bull",
                           static_cast<int>(ThemeType::RacingRed));
    themeSelector->addItem("💎 Luxury", static_cast<int>(ThemeType::Luxury));
    themeSelector->addItem("🌊 Ocean Blue",
                           static_cast<int>(ThemeType::OceanBlue));
    themeSelector->addItem("💥 Post-Apocalypse",
                           static_cast<int>(ThemeType::PostApocalypse));
    themeSelector->setMinimumWidth(200);

    connect(themeSelector, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &SearchingWindow::onThemeChanged);

    toolbarLayout->addWidget(themeSelector);
    toolbarLayout->addStretch();
    layout->addLayout(toolbarLayout);

    // Título
    titleLabel = new QLabel("🏁 Lobby - Need for Speed 🏁", this);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(18);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    titleLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(titleLabel);

    // Label de estado
    statusLabel = new QLabel("Conectado al servidor", this);
    statusLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(statusLabel);

    // Lista de partidas
    gamesListWidget = new QListWidget(this);
    gamesListWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    gamesListWidget->setMinimumHeight(300);
    layout->addWidget(gamesListWidget);

    connect(gamesListWidget, &QListWidget::itemClicked, this,
            &SearchingWindow::onGameSelected);
    connect(gamesListWidget, &QListWidget::itemDoubleClicked, this,
            &SearchingWindow::onGameDoubleClicked);

    // Botones de acción
    QHBoxLayout* buttonLayout = new QHBoxLayout();

    createGameButton = new QPushButton("➕ Crear Partida", this);
    createGameButton->setMinimumHeight(40);
    connect(createGameButton, &QPushButton::clicked, this,
            &SearchingWindow::onCreateGameClicked);
    buttonLayout->addWidget(createGameButton);

    joinGameButton = new QPushButton("🎮 Unirse a Partida", this);
    joinGameButton->setMinimumHeight(40);
    joinGameButton->setEnabled(false);
    connect(joinGameButton, &QPushButton::clicked, this,
            &SearchingWindow::onJoinGameClicked);
    buttonLayout->addWidget(joinGameButton);

    refreshButton = new QPushButton("🔄 Actualizar", this);
    refreshButton->setMinimumHeight(40);
    connect(refreshButton, &QPushButton::clicked, this,
            &SearchingWindow::onRefreshClicked);
    buttonLayout->addWidget(refreshButton);

    layout->addLayout(buttonLayout);
}

// ========== MODIFICACIÓN DEL SLOT ORIGINAL ==========

void SearchingWindow::onCreateGameClicked() {
    // showCreateGamePage();
    emit createGameClicked();
}
