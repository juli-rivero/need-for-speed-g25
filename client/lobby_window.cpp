#include "lobby_window.h"
#include "theme_manager.h"
#include <QMessageBox>
#include <QProgressBar>
#include <QFrame>
#include <QToolBar>
#include <QFormLayout>
#include <QRegularExpression>

LobbyWindow::LobbyWindow(QWidget *parent)
    : QMainWindow(parent), currentGameId(-1) {
    
    lobbyClient = new MockLobbyClient();
    
    // Conectar señales del cliente
    connect(lobbyClient, &ILobbyClient::connected, 
            this, &LobbyWindow::onConnected);
    connect(lobbyClient, &ILobbyClient::gamesListReceived, 
            this, &LobbyWindow::onGamesListReceived);
    connect(lobbyClient, &ILobbyClient::gameCreated, 
            this, &LobbyWindow::onGameCreated);
    connect(lobbyClient, &ILobbyClient::gameJoined, 
            this, &LobbyWindow::onGameJoined);
    connect(lobbyClient, &ILobbyClient::playersListUpdated,
            this, &LobbyWindow::onPlayersListUpdated);
    connect(lobbyClient, &ILobbyClient::gameStarting,
            this, &LobbyWindow::onGameStarting);
    connect(lobbyClient, &ILobbyClient::error, 
            this, &LobbyWindow::onError);
    
    setupUI();
    
    // Conectar señales del create game widget
    connect(createGameWidget, &CreateGameDialog::submitRequested,
            this, &LobbyWindow::onCreateGameSubmit);
    connect(createGameWidget, &CreateGameDialog::cancelRequested,
            this, &LobbyWindow::onCreateGameCancel);
    
    // Conectar señales del car selection widget
    connect(carSelectionWidget, &CarSelectionDialog::confirmRequested,
            this, &LobbyWindow::onCarSelectionConfirm);
    connect(carSelectionWidget, &CarSelectionDialog::cancelRequested,
            this, &LobbyWindow::onCarSelectionCancel);
    
    // Conectar señales del waiting room widget
    connect(waitingRoomWidget, &WaitingRoomWidget::leaveGameRequested,
            this, &LobbyWindow::onWaitingRoomLeaveRequested);
    connect(waitingRoomWidget, &WaitingRoomWidget::readyStateChanged,
            this, &LobbyWindow::onWaitingRoomReadyChanged);
    connect(waitingRoomWidget, &WaitingRoomWidget::startGameRequested,
            this, &LobbyWindow::onWaitingRoomStartGame);

    // Conectar el cambio de tema global
    connect(&ThemeManager::instance(), &ThemeManager::themeChanged,
            this, &LobbyWindow::applyTheme);
    
    // Aplicar tema inicial
    applyTheme();
    
    lobbyClient->connectToServer("localhost", 8080);
}

LobbyWindow::~LobbyWindow() {
    // Desconectar del servidor
    if (lobbyClient) {
        lobbyClient->disconnect();
        delete lobbyClient;
    }
}

void LobbyWindow::setupUI() {
    // Configurar ventana principal
    setWindowTitle("Need for Speed - Lobby");
    setMinimumSize(1000, 700);

    // Crear stack de páginas
    stackedWidget = new QStackedWidget(this);
    setCentralWidget(stackedWidget);

    // Usando widgets modulares
    lobbyPage = createLobbyPage();
    createGameWidget = new CreateGameDialog(this);      
    carSelectionWidget = new CarSelectionDialog(this);  
    waitingRoomWidget = new WaitingRoomWidget(this);    
    
    // Agregar páginas al stack
    stackedWidget->addWidget(lobbyPage);              // Índice 0
    stackedWidget->addWidget(createGameWidget);       // Índice 1
    stackedWidget->addWidget(carSelectionWidget);     // Índice 2
    stackedWidget->addWidget(waitingRoomWidget);      // Índice 3
    
    // Mostrar lobby por defecto
    stackedWidget->setCurrentIndex(0);
}

void LobbyWindow::updateGamesList(const std::vector<GameInfo>& games) {
    currentGames = games;
    
    // Limpiar lista actual
    gamesListWidget->clear();
    
    // Si no hay partidas, mostrar mensaje
    if (games.empty()) {
        QListWidgetItem* emptyItem = new QListWidgetItem("No hay partidas disponibles");
        emptyItem->setFlags(Qt::ItemIsEnabled); // No seleccionable
        gamesListWidget->addItem(emptyItem);
        return;
    }
    
    // Agregar cada partida usando GameCardWidget
    for (const GameInfo& game : games) {
        QListWidgetItem* item = new QListWidgetItem(gamesListWidget);
        item->setData(Qt::UserRole, game.id); // Guardar ID de la partida
        
        GameCardWidget* gameCard = new GameCardWidget(game);
        item->setSizeHint(gameCard->sizeHint());
        
        gamesListWidget->addItem(item);
        gamesListWidget->setItemWidget(item, gameCard);
    }
}

// NAVEGACIÓN ENTRE PÁGINAS

void LobbyWindow::onJoinGameClicked() {
    QListWidgetItem *selectedItem = gamesListWidget->currentItem();
    if (selectedItem) {
        int gameId = selectedItem->data(Qt::UserRole).toInt();
        
        // Verificar que no esté llena
        auto it = std::find_if(currentGames.begin(), currentGames.end(),
            [gameId](const GameInfo& g) { return g.id == gameId; });
        
        if (it != currentGames.end() && it->status == "full") {
            QMessageBox::warning(this, "Partida Llena", 
                               "Esta partida ya está llena. Elige otra.");
            return;
        }
        
        statusLabel->setText("Uniéndose a la partida...");
        statusLabel->setStyleSheet("color: orange;");
        lobbyClient->joinGame(gameId);
    }
}

void LobbyWindow::onRefreshClicked() {
    statusLabel->setText("Actualizando...");
    statusLabel->setStyleSheet("color: orange;");
    
    // Solicitar lista actualizada al servidor
    lobbyClient->requestGamesList();
}

void LobbyWindow::onGameSelected(QListWidgetItem *item) {
    // Habilitar botón de unirse cuando se selecciona una partida
    joinGameButton->setEnabled(item != nullptr);
}

void LobbyWindow::onGameDoubleClicked(QListWidgetItem *item) {
    if (!item) return;
    
    int gameId = item->data(Qt::UserRole).toInt();
    
    // Verificar que no esté llena o en juego
    auto it = std::find_if(currentGames.begin(), currentGames.end(),
        [gameId](const GameInfo& g) { return g.id == gameId; });
    
    if (it != currentGames.end()) {
        if (it->status == "full") {
            QMessageBox::warning(this, "Partida Llena", 
                               "Esta partida ya está llena. Elige otra.");
            return;
        }
        if (it->status == "playing") {
            QMessageBox::warning(this, "Partida en Curso", 
                               "Esta partida ya comenzó. No puedes unirte.");
            return;
        }
    }
    
    // Unirse directamente con doble click
    statusLabel->setText("Uniéndose a la partida...");
    statusLabel->setStyleSheet("color: orange;");
    lobbyClient->joinGame(gameId);
}

// Implementación de slots para respuestas del cliente
void LobbyWindow::onConnected() {
    statusLabel->setText("✅ Conectado al servidor");
    statusLabel->setStyleSheet("color: green;");
    
    // Solicitar lista de partidas al conectar
    lobbyClient->requestGamesList();
}

void LobbyWindow::onGamesListReceived(std::vector<GameInfo> games) {
    updateGamesList(games);
    
    // Feedback visual de actualización exitosa
    statusLabel->setText(QString("✅ Lista actualizada (%1 partidas)").arg(games.size()));
    statusLabel->setStyleSheet("color: green;");
}

void LobbyWindow::onGameCreated(int gameId) {
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
}

void LobbyWindow::onGameJoined(int gameId) {
    // Guardar el ID de la partida a la que nos unimos
    joiningGameId = gameId;
    
    // Establecer modo: estamos UNIÉNDONOS a una partida
    carSelectionMode = CarSelectionMode::Joining;
    
    // Cuando te unes a una partida, ir a selección de auto
    showCarSelectionPage();
    
    statusLabel->setText("✅ Unido a la partida - Selecciona tu auto");
    statusLabel->setStyleSheet("color: green;");
}

void LobbyWindow::onError(QString message) {
    statusLabel->setText("❌ Error: " + message);
    statusLabel->setStyleSheet("color: red;");
    
    QMessageBox::critical(this, "Error", message);
}
QString LobbyWindow::getCarEmoji(const QString& carType) const {
    if (carType == "Deportivo") return "🏎️";
    if (carType == "Sedán") return "🚗";
    if (carType == "SUV") return "🚙";
    if (carType == "Camión") return "🚚";
    if (carType == "Muscle Car") return "🚗";
    if (carType == "Compacto") return "🚕";
    return "🚗";
}

// GESTIÓN DE TEMAS DINÁMICOS

void LobbyWindow::onThemeChanged(int index) {
    if (index < 0 || !themeSelector) return;
    
    // Obtener el tipo de tema del combo box
    ThemeType selectedTheme = static_cast<ThemeType>(themeSelector->itemData(index).toInt());
    
    // Actualizar el ThemeManager (esto disparará la señal themeChanged)
    ThemeManager::instance().setTheme(selectedTheme);
}

void LobbyWindow::applyTheme() {
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
    
    // Actualizar fondo del stackedWidget
    if (stackedWidget) {
        stackedWidget->setStyleSheet(QString(
            "QWidget {"
            "    background-color: %1;"
            "}"
        ).arg(palette.cardBackgroundHover));
    }
    
    // Actualizar título
    if (titleLabel) {
        titleLabel->setStyleSheet(QString(
            "color: %1;"
            "font-size: 18px;"
            "font-weight: bold;"
        ).arg(palette.textPrimary));
    }
    
    // Actualizar labels
    if (statusLabel) {
        // Mantener el color del status actual (verde/rojo/naranja) pero actualizar fuente
        QString currentStyle = statusLabel->styleSheet();
        if (!currentStyle.contains("color:")) {
            statusLabel->setStyleSheet(QString("color: %1;").arg(palette.textSecondary));
        }
    }
    
    // Actualizar toolbar (si existe)
    QList<QToolBar*> toolbars = findChildren<QToolBar*>();
    for (QToolBar* toolbar : toolbars) {
        toolbar->setStyleSheet(QString(
            "QToolBar {"
            "    background-color: %1;"
            "    border-bottom: 2px solid %2;"
            "    padding: 5px;"
            "}"
            "QLabel {"
            "    color: %3;"
            "}"
        ).arg(palette.cardBackground)
          .arg(palette.primaryColor)
          .arg(palette.textPrimary));
    }
    
    // Actualizar selector de temas
    if (themeSelector) {
        themeSelector->setStyleSheet(QString(
            "QComboBox {"
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
            "}"
        ).arg(palette.cardBackground)
          .arg(palette.textPrimary)
          .arg(palette.primaryColor)
          .arg(palette.secondaryColor));
    }
    
    // Recrear las tarjetas de juegos con el nuevo tema
    updateGamesList(currentGames);
}

// ========== MÉTODOS PARA CREAR PÁGINAS ==========

QWidget* LobbyWindow::createLobbyPage() {
    QWidget* page = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(page);
    
    // ===== Toolbar con selector de tema =====
    QHBoxLayout* toolbarLayout = new QHBoxLayout();
    
    QLabel* themeLabel = new QLabel("🎨 Tema:", page);
    toolbarLayout->addWidget(themeLabel);
    
    themeSelector = new QComboBox(page);
    themeSelector->addItem("🎨 Default", static_cast<int>(ThemeType::Default));
    themeSelector->addItem("🌙 Dark Mode", static_cast<int>(ThemeType::DarkMode));
    themeSelector->addItem("🏎️ Racing Bull", static_cast<int>(ThemeType::RacingRed));
    themeSelector->addItem("💎 Luxury", static_cast<int>(ThemeType::Luxury));
    themeSelector->addItem("🌊 Ocean Blue", static_cast<int>(ThemeType::OceanBlue));
    themeSelector->addItem("💥 Post-Apocalypse", static_cast<int>(ThemeType::PostApocalypse));
    themeSelector->setMinimumWidth(200);
    
    connect(themeSelector, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &LobbyWindow::onThemeChanged);
    
    toolbarLayout->addWidget(themeSelector);
    toolbarLayout->addStretch();
    layout->addLayout(toolbarLayout);
    
    // Título
    titleLabel = new QLabel("🏁 Lobby - Need for Speed 🏁", page);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(18);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    titleLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(titleLabel);
    
    // Label de estado
    statusLabel = new QLabel("Conectado al servidor", page);
    statusLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(statusLabel);
    
    // Lista de partidas
    gamesListWidget = new QListWidget(page);
    gamesListWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    gamesListWidget->setMinimumHeight(300);
    layout->addWidget(gamesListWidget);
    
    connect(gamesListWidget, &QListWidget::itemClicked,
            this, &LobbyWindow::onGameSelected);
    connect(gamesListWidget, &QListWidget::itemDoubleClicked,
            this, &LobbyWindow::onGameDoubleClicked);
    
    // Botones de acción
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    
    createGameButton = new QPushButton("➕ Crear Partida", page);
    createGameButton->setMinimumHeight(40);
    connect(createGameButton, &QPushButton::clicked,
            this, &LobbyWindow::onCreateGameClicked);
    buttonLayout->addWidget(createGameButton);
    
    joinGameButton = new QPushButton("🎮 Unirse a Partida", page);
    joinGameButton->setMinimumHeight(40);
    joinGameButton->setEnabled(false);
    connect(joinGameButton, &QPushButton::clicked,
            this, &LobbyWindow::onJoinGameClicked);
    buttonLayout->addWidget(joinGameButton);
    
    refreshButton = new QPushButton("🔄 Actualizar", page);
    refreshButton->setMinimumHeight(40);
    connect(refreshButton, &QPushButton::clicked,
            this, &LobbyWindow::onRefreshClicked);
    buttonLayout->addWidget(refreshButton);
    
    layout->addLayout(buttonLayout);
    
    return page;
}

// ========== NAVEGACIÓN ENTRE PÁGINAS ==========

void LobbyWindow::showLobbyPage() {
    stackedWidget->setCurrentIndex(0);
    setWindowTitle("Need for Speed - Lobby");
}

void LobbyWindow::showCreateGamePage() {
    stackedWidget->setCurrentIndex(1);
    setWindowTitle("Need for Speed - Crear Partida");
    createGameWidget->reset();  // Limpiar formulario
}

void LobbyWindow::showCarSelectionPage() {
    stackedWidget->setCurrentIndex(2);
    setWindowTitle("Need for Speed - Seleccionar Auto");
    carSelectionWidget->reset();  // Resetear selección
}

// ========== SLOTS DE FORMULARIO DE CREAR PARTIDA ==========

void LobbyWindow::onCreateGameSubmit() {
    // Obtener configuración del widget
    auto config = createGameWidget->getConfig();
    
    // Validar nombre (el widget ya debería validar, pero por seguridad)
    if (config.name.isEmpty()) {
        QMessageBox::warning(this, "Error", "Debes ingresar un nombre para la partida");
        return;
    }
    
    // Establecer modo: estamos CREANDO una partida
    carSelectionMode = CarSelectionMode::Creating;
    
    // Ir a selección de auto
    showCarSelectionPage();
}

void LobbyWindow::onCreateGameCancel() {
    showLobbyPage();
}

// ========== SLOTS DE SELECCIÓN DE AUTO ==========

void LobbyWindow::onCarSelectionConfirm() {
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
        setWindowTitle("Need for Speed - Sala de Espera");
        
        statusLabel->setText("✅ Te has unido a la partida");
        statusLabel->setStyleSheet("color: green;");
    }
}

void LobbyWindow::onCarSelectionCancel() {
    if (carSelectionMode == CarSelectionMode::Creating) {
        // Si estábamos creando, volver al formulario
        showCreateGamePage();
    } else {
        // Si estábamos uniéndonos, volver al lobby
        showLobbyPage();
        statusLabel->setText("❌ Cancelado - No te uniste a la partida");
        statusLabel->setStyleSheet("color: orange;");
    }
}

// ========== MODIFICACIÓN DEL SLOT ORIGINAL ==========

void LobbyWindow::onCreateGameClicked() {
    showCreateGamePage();
}

// ========== SLOTS DELEGADOS DE SALA DE ESPERA ==========

void LobbyWindow::onWaitingRoomLeaveRequested() {
    lobbyClient->leaveGame();
    showLobbyPage();
    statusLabel->setText("Has salido de la partida");
    statusLabel->setStyleSheet("color: orange;");
}

void LobbyWindow::onWaitingRoomReadyChanged(bool ready) {
    lobbyClient->setReady(ready);
}

void LobbyWindow::onWaitingRoomStartGame() {
    QMessageBox::information(this, "🏁 ¡La Carrera Comienza!", 
                            "¡Todos los jugadores están listos!\n\n"
                            "La carrera comenzará en breve...");
    
    // TODO: Iniciar ventana del juego
    showLobbyPage();
    lobbyClient->requestGamesList();
}

void LobbyWindow::onPlayersListUpdated(std::vector<PlayerInfo> players) {
    waitingRoomWidget->updatePlayersList(players);
}


void LobbyWindow::onGameStarting() {
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
}
