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
                                       const std::vector<CarInfo>&) {
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
    // Obtener el tema actual
    ThemeManager& theme = ThemeManager::instance();
    const ColorPalette& palette = theme.getCurrentPalette();

    // Actualizar estilos de botones
    if (createGameButton) {
        createGameButton->setStyleSheet(theme.buttonPrimaryStyle());
    }
    if (joinGameButton) {
        joinGameButton->setStyleSheet(theme.buttonSecondaryStyle());
    }
    if (refreshButton) {
        refreshButton->setStyleSheet(theme.buttonActionStyle());
    }

    // Actualizar título
    if (titleLabel) {
        titleLabel->setStyleSheet(QString("color: %1;"
                                          "font-size: 18px;"
                                          "font-weight: bold;")
                                      .arg(palette.textPrimary));
    }

    // Actualizar labels
    if (statusLabel) {
        // Mantener el color del status actual (verde/rojo/naranja) pero
        // actualizar fuente
        QString currentStyle = statusLabel->styleSheet();
        if (!currentStyle.contains("color:")) {
            statusLabel->setStyleSheet(
                QString("color: %1;").arg(palette.textSecondary));
        }
    }

    // Actualizar toolbar (si existe)
    QList<QToolBar*> toolbars = findChildren<QToolBar*>();
    for (QToolBar* toolbar : toolbars) {
        toolbar->setStyleSheet(QString("QToolBar {"
                                       "    background-color: %1;"
                                       "    border-bottom: 2px solid %2;"
                                       "    padding: 5px;"
                                       "}"
                                       "QLabel {"
                                       "    color: %3;"
                                       "}")
                                   .arg(palette.cardBackground)
                                   .arg(palette.primaryColor)
                                   .arg(palette.textPrimary));
    }

    // Actualizar selector de temas
    if (themeSelector) {
        themeSelector->setStyleSheet(
            QString("QComboBox {"
                    "    background-color: %1;"
                    "    color: %2;"
                    "    border: 2px solid %3;"
                    "    border-radius: 4px;"
                    "    padding: 5px;"
                    "    min-width: 200px;"
                    "}"
                    "QComboBox:hover {"
                    "    border-color: %4;"
                    "}"
                    "QComboBox::drop-down {"
                    "    border: none;"
                    "}"
                    "QComboBox QAbstractItemView {"
                    "    background-color: %1;"
                    "    color: %2;"
                    "    selection-background-color: %4;"
                    "    selection-color: white;"
                    "}")
                .arg(palette.cardBackground)
                .arg(palette.textPrimary)
                .arg(palette.primaryColor)
                .arg(palette.secondaryColor));
    }

    // Recrear las tarjetas de juegos con el nuevo tema
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
