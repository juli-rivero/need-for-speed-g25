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
    : QWidget(parent), connexion(connexion), api(connexion), currentGameId(-1) {
    spdlog::trace("creating lobby window");
    // lobbyClient = new MockLobbyClient();
    // spdlog::trace("created lobby client");

    // TODO(juli): borrar y cambiar por mock connexion de la rama base
    // Conectar señales del cliente
    // connect(lobbyClient, &ILobbyClient::connected, this,
    //        &SearchingWindow::onConnected);
    // connect(lobbyClient, &ILobbyClient::gamesListReceived, this,
    //        &SearchingWindow::onGamesListReceived);
    // connect(lobbyClient, &ILobbyClient::gameCreated,
    //         this, &SearchingWindow::onGameCreated);
    // connect(lobbyClient, &ILobbyClient::gameJoined, this,
    //        &SearchingWindow::onGameJoined);
    // connect(lobbyClient, &ILobbyClient::playersListUpdated,
    //        this, &SearchingWindow::onPlayersListUpdated);
    // connect(lobbyClient, &ILobbyClient::gameStarting,
    //        this, &SearchingWindow::onGameStarting);
    // connect(lobbyClient, &ILobbyClient::error, this,
    // &SearchingWindow::onError);

    // TODO(juli): cambiar
    /*// Conectar señales del create game widget
    connect(createGameWidget, &CreateGameDialog::submitRequested,
            this, &SearchingWindow::onCreateGameSubmit);
    connect(createGameWidget, &CreateGameDialog::cancelRequested,
            this, &SearchingWindow::onCreateGameCancel);

    // Conectar señales del car selection widget
    connect(carSelectionWidget, &CarSelectionDialog::confirmRequested,
            this, &SearchingWindow::onCarSelectionConfirm);
    connect(carSelectionWidget, &CarSelectionDialog::cancelRequested,
            this, &SearchingWindow::onCarSelectionCancel);

    // Conectar señales del waiting room widget
    connect(waitingRoomWidget, &WaitingRoomWidget::leaveGameRequested,
            this, &SearchingWindow::onWaitingRoomLeaveRequested);
    connect(waitingRoomWidget, &WaitingRoomWidget::readyStateChanged,
            this, &SearchingWindow::onWaitingRoomReadyChanged);
    connect(waitingRoomWidget, &WaitingRoomWidget::startGameRequested,
            this, &SearchingWindow::onWaitingRoomStartGame);*/
    // spdlog::trace("connected signals");

    // Conectar el cambio de tema global
    connect(&ThemeManager::instance(), &ThemeManager::themeChanged, this,
            &SearchingWindow::applyTheme);

    createThis();
    // Aplicar tema inicial
    applyTheme();

    connexion.control(*this);
    api.request_all_sessions();
}

SearchingWindow::~SearchingWindow() { connexion.decontrol(*this); }

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

        api.request_join(gameId);

        statusLabel->setText("Uniéndose a la partida...");
        statusLabel->setStyleSheet("color: orange;");
    }
}

void SearchingWindow::onRefreshClicked() {
    statusLabel->setText("Actualizando...");
    statusLabel->setStyleSheet("color: orange;");

    // Solicitar lista actualizada al servidor
    api.request_all_sessions();
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
    api.request_join(gameId);

    // Unirse directamente con doble click
    statusLabel->setText("Uniéndose a la partida...");
    statusLabel->setStyleSheet("color: orange;");
}

/*// Implementación de slots para respuestas del cliente
void SearchingWindow::onConnected() { // ya esta conectado a la hora de crearse
el widget statusLabel->setText("✅ Conectado al servidor");
    statusLabel->setStyleSheet("color: green;");

    // Solicitar lista de partidas al conectar
    lobbyClient->requestGamesList();
}*/

/*void SearchingWindow::onGamesListReceived(std::vector<GameInfo> games) {
    updateGamesList(games);

    // Feedback visual de actualización exitosa
    statusLabel->setText(
        QString("✅ Lista actualizada (%1 partidas)").arg(games.size()));
    statusLabel->setStyleSheet("color: green;");
}*/

/*void SearchingWindow::onGameCreated(int gameId) {
    currentGameId = gameId;

    statusLabel->setText("✅ Partida creada exitosamente");
    statusLabel->setStyleSheet("color: green;");

    int selectedCarType = carSelectionWidget->getSelectedCarType();

    // Simular lista de jugadores inicial (tú como host)
    std::vector<PlayerInfo> players = {
        {1, "Tú", selectedCarType, false, true},  // Host
    };

    // Configurar y mostrar waiting room
    waitingRoomWidget->setGameInfo(gameId, players);
    stackedWidget->setCurrentIndex(3);
    setWindowTitle("Need for Speed - Sala de Espera");
}*/

void SearchingWindow::joinGame(const std::string&) {
    // Guardar el ID de la partida a la que nos unimos
    // joiningGameId = gameId; se guarda en el servidor, se puede usar algun
    // request para pedirlo

    // Establecer modo: estamos UNIÉNDONOS a una partida
    // carSelectionMode = CarSelectionMode::Joining; # TODO(nico): verificar si
    // se puede borrar

    // Cuando te unes a una partida, ir a selección de auto
    // showCarSelectionPage();
    emit joinGameClicked();

    statusLabel->setText("✅ Unido a la partida - Selecciona tu auto");
    statusLabel->setStyleSheet("color: green;");
}

void SearchingWindow::onError(QString message) {
    statusLabel->setText("❌ Error: " + message);
    statusLabel->setStyleSheet("color: red;");

    QMessageBox::critical(this, "Error", message);
}
QString SearchingWindow::getCarEmoji(const QString& carType) const {
    if (carType == "Deportivo") return "🏎️";
    if (carType == "Sedán") return "🚗";
    if (carType == "SUV") return "🚙";
    if (carType == "Camión") return "🚚";
    if (carType == "Muscle Car") return "🚗";
    if (carType == "Compacto") return "🚕";
    return "🚗";
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

// ========== SLOTS DE FORMULARIO DE CREAR PARTIDA ==========

/*void SearchingWindow::onCreateGameSubmit() {
    // Obtener configuración del widget
    auto config = createGameWidget->getConfig();

    // Validar nombre (el widget ya debería validar, pero por seguridad)
    if (config.name.isEmpty()) {
        QMessageBox::warning(this, "Error", "Debes ingresar un nombre para la
partida"); return;
    }

    // Establecer modo: estamos CREANDO una partida
    carSelectionMode = CarSelectionMode::Creating;

    // Ir a selección de auto
    showCarSelectionPage();
}*/

/*void SearchingWindow::onCreateGameCancel() {
    showLobbyPage();
}*/

// ========== SLOTS DE SELECCIÓN DE AUTO ==========

/*void SearchingWindow::onCarSelectionConfirm() {
    int selectedCarType = carSelectionWidget->getSelectedCarType();

    if (selectedCarType == -1) {
        QMessageBox::warning(this, "Error", "Debes seleccionar un auto");
        return;
    }

    if (carSelectionMode == CarSelectionMode::Creating) {
        // MODO: Crear nueva partida
        auto config = createGameWidget->getConfig();

        GameConfig gameConfig;
        gameConfig.name = config.name;
        gameConfig.maxPlayers = config.maxPlayers;
        gameConfig.raceCount = config.raceCount;
        gameConfig.lapCount = config.lapCount;
        gameConfig.city = config.city;
        gameConfig.carType = selectedCarType;

        lobbyClient->createGame(gameConfig);

        // No volver al lobby, onGameCreated() nos llevará a la sala de espera
        statusLabel->setText("⏳ Creando partida...");

    } else if (carSelectionMode == CarSelectionMode::Joining) {
        // MODO: Unirse a partida existente

        // TODO: Cuando implementes el protocolo real, agregar:
        // lobbyClient->selectCar(joiningGameId, selectedCarType);

        // Simular unión exitosa - ir a sala de espera
        std::vector<PlayerInfo> players = {
            {1, "Host", 1, true, true},        // Host (ready)
            {2, "Tú", selectedCarType, false, false},  // Tú (not ready)
            {3, "Player3", 3, false, false},   // Otro jugador
        };
        currentGameId = joiningGameId;

        // Configurar y mostrar waiting room
        waitingRoomWidget->setGameInfo(currentGameId, players);
        stackedWidget->setCurrentIndex(3);

        statusLabel->setText("✅ Te has unido a la partida");
        statusLabel->setStyleSheet("color: green;");
    }
}*/

/*void SearchingWindow::onCarSelectionCancel() {
    if (carSelectionMode == CarSelectionMode::Creating) {
        // Si estábamos creando, volver al formulario
        showCreateGamePage();
    } else {
        // Si estábamos uniéndonos, volver al lobby
        showLobbyPage();
        statusLabel->setText("❌ Cancelado - No te uniste a la partida");
        statusLabel->setStyleSheet("color: orange;");
    }
}*/

// ========== MODIFICACIÓN DEL SLOT ORIGINAL ==========

void SearchingWindow::onCreateGameClicked() {
    // showCreateGamePage();
    emit createGameClicked();
}

// ========== SLOTS DELEGADOS DE SALA DE ESPERA ==========

/*void SearchingWindow::onWaitingRoomLeaveRequested() {
    lobbyClient->leaveGame();
    showLobbyPage();
    statusLabel->setText("Has salido de la partida");
    statusLabel->setStyleSheet("color: orange;");
}*/

/*void SearchingWindow::onWaitingRoomReadyChanged(bool ready) {
    lobbyClient->setReady(ready);
}*/

/*void SearchingWindow::onWaitingRoomStartGame() {
    QMessageBox::information(this, "🏁 ¡La Carrera Comienza!",
                            "¡Todos los jugadores están listos!\n\n"
                            "La carrera comenzará en breve...");

    // TODO: Iniciar ventana del juego
    showLobbyPage();
    lobbyClient->requestGamesList();
}*/

/*void SearchingWindow::onPlayersListUpdated(std::vector<PlayerInfo> players) {
    waitingRoomWidget->updatePlayersList(players);
}*/

/*void SearchingWindow::onGameStarting() {
    QMessageBox::information(this, "🏁 ¡La Carrera Comienza!",
                            "¡Todos los jugadores están listos!\n\n"
                            "La carrera comenzará en breve...");

    // TODO: Aquí iniciar la ventana del juego SDL2
    // gameWindow = new GameWindow(currentGameId);
    // gameWindow->show();
    // this->hide();

    // Por ahora, volver al lobby
    showLobbyPage();
    lobbyClient->requestGamesList();
}*/
