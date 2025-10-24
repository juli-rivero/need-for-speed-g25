#include "lobby_window.h"
#include "theme_manager.h"
#include "create_game_dialog.h"
#include "car_selection_dialog.h"
#include <QMessageBox>
#include <QProgressBar>
#include <QFrame>
#include <QToolBar>

LobbyWindow::LobbyWindow(QWidget *parent)
    : QMainWindow(parent) {
    
    // Cambiar de:
    //     lobbyClient = new MockLobbyClient();
    // 
    // A:
    //     lobbyClient = new RealLobbyClient();
    // 
    // RealLobbyClient debe:
    //   1. Heredar de ILobbyClient (ver client/lobby_client.h)
    //   2. Implementar los 5 métodos virtuales
    //   3. Emitir las señales correctas cuando reciba datos del servidor

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
    connect(lobbyClient, &ILobbyClient::error, 
            this, &LobbyWindow::onError);
    
    setupUI();

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
    setMinimumSize(800, 600);

    // ===== Toolbar con selector de tema =====
    QToolBar* toolbar = new QToolBar("Opciones", this);
    toolbar->setMovable(false);
    addToolBar(Qt::TopToolBarArea, toolbar);
    
    // Selector de tema
    QLabel* themeLabel = new QLabel("  🎨 Tema: ", toolbar);
    toolbar->addWidget(themeLabel);
    
    themeSelector = new QComboBox(toolbar);
    themeSelector->addItem("🎨 Default", static_cast<int>(ThemeType::Default));
    themeSelector->addItem("🌙 Dark Mode", static_cast<int>(ThemeType::DarkMode));
    themeSelector->addItem("🏎️ Racing Bull", static_cast<int>(ThemeType::RacingRed));
    themeSelector->addItem("💎 Luxury", static_cast<int>(ThemeType::Luxury));
    themeSelector->addItem("🌊 Ocean Blue", static_cast<int>(ThemeType::OceanBlue));
    themeSelector->addItem("💥 Post-Apocalypse", static_cast<int>(ThemeType::PostApocalypse));
    themeSelector->setMinimumWidth(250);
    
    connect(themeSelector, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &LobbyWindow::onThemeChanged);
    
    toolbar->addWidget(themeSelector);
    toolbar->addSeparator();

    // Widget central
    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    // Layout principal
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

    // Título
    titleLabel = new QLabel("🏁 Lobby - Need for Speed 🏁", this);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(18);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titleLabel);

    // Label de estado
    statusLabel = new QLabel("Conectado al servidor", this);
    statusLabel->setStyleSheet("color: green;");
    statusLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(statusLabel);
    
    // Lista de partidas
    QLabel *gamesLabel = new QLabel("Partidas Disponibles:", this);
    mainLayout->addWidget(gamesLabel);

    gamesListWidget = new QListWidget(this);
    gamesListWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    mainLayout->addWidget(gamesListWidget);

    // Conectar señales de selección y doble click
    connect(gamesListWidget, &QListWidget::itemClicked, 
            this, &LobbyWindow::onGameSelected);
    connect(gamesListWidget, &QListWidget::itemDoubleClicked, 
            this, &LobbyWindow::onGameDoubleClicked);

    // Layout para botones
    QHBoxLayout *buttonsLayout = new QHBoxLayout();

    // Botón Crear Partida
    createGameButton = new QPushButton("🎮 Crear Partida", this);
    createGameButton->setMinimumHeight(40);
    connect(createGameButton, &QPushButton::clicked, 
            this, &LobbyWindow::onCreateGameClicked);
    buttonsLayout->addWidget(createGameButton);

    // Botón Unirse a Partida
    joinGameButton = new QPushButton("🚗 Unirse a Partida", this);
    joinGameButton->setMinimumHeight(40);
    joinGameButton->setEnabled(false); // Deshabilitado hasta seleccionar una partida
    connect(joinGameButton, &QPushButton::clicked, 
            this, &LobbyWindow::onJoinGameClicked);
    buttonsLayout->addWidget(joinGameButton);

    // Botón Actualizar
    refreshButton = new QPushButton("🔄 Actualizar", this);
    refreshButton->setMinimumHeight(40);
    connect(refreshButton, &QPushButton::clicked, 
            this, &LobbyWindow::onRefreshClicked);
    buttonsLayout->addWidget(refreshButton);

    mainLayout->addLayout(buttonsLayout);
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
    
    // Agregar cada partida como un widget personalizado
    for (const GameInfo& game : games) {
        QListWidgetItem* item = new QListWidgetItem(gamesListWidget);
        item->setData(Qt::UserRole, game.id); // Guardar ID de la partida
        
        QWidget* gameWidget = createGameItemWidget(game);
        item->setSizeHint(gameWidget->sizeHint());
        
        gamesListWidget->addItem(item);
        gamesListWidget->setItemWidget(item, gameWidget);
    }
}

QString LobbyWindow::getMapIcon(const QString& mapName) const {
    if (mapName == "city") return "🏙️";
    return "🏎️";
}

void LobbyWindow::onCreateGameClicked() {
    // Crear y mostrar el diálogo
    CreateGameDialog dialog(this);
    
    // Ejecutar el diálogo (modal)
    if (dialog.exec() == QDialog::Accepted) {
        // Obtener la configuración
        CreateGameDialog::GameConfig dialogConfig = dialog.getConfig();
        
        // Convertir a GameConfig para el cliente
        GameConfig config;
        config.name = dialogConfig.name;
        config.maxPlayers = dialogConfig.maxPlayers;
        config.raceCount = dialogConfig.raceCount;
        config.city = dialogConfig.city;
        
        // Enviar al servidor
        statusLabel->setText("Creando partida...");
        statusLabel->setStyleSheet("color: orange;");
        lobbyClient->createGame(config);
    }
}

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
    Q_UNUSED(gameId);
    
    // Mostrar diálogo de selección de auto
    CarSelectionDialog carDialog(this);
    if (carDialog.exec() == QDialog::Accepted) {
        CarConfig selectedCar = carDialog.getSelectedCar();
        
        QMessageBox::information(this, "✅ Partida Creada", 
                                QString("Tu partida ha sido creada exitosamente!\n\n"
                                        "Auto seleccionado: %1 %2\n"
                                        "Esperando que otros jugadores se unan...")
                                .arg(selectedCar.carType)
                                .arg(getCarEmoji(selectedCar.carType)));
        // Agregar a la interfaz ILobbyClient:
        //     virtual void selectCar(int gameId, const CarConfig& car) = 0;
        // 
        // Y emitir señal cuando se confirme:
        //     emit carSelected(int gameId);
        // 
        // Descomentar esta línea cuando esté implementado:
        // lobbyClient->selectCar(gameId, selectedCar);
    }
    
    // Actualizar lista
    lobbyClient->requestGamesList();
}

void LobbyWindow::onGameJoined(int gameId) {
    Q_UNUSED(gameId);
    
    // Mostrar diálogo de selección de auto
    CarSelectionDialog carDialog(this);
    if (carDialog.exec() == QDialog::Accepted) {
        CarConfig selectedCar = carDialog.getSelectedCar();
        
        QMessageBox::information(this, "✅ Unido a Partida", 
                                QString("Te has unido a la partida exitosamente!\n\n"
                                        "Auto seleccionado: %1 %2\n"
                                        "Esperando que la partida comience...")
                                .arg(selectedCar.carType)
                                .arg(getCarEmoji(selectedCar.carType)));
        
        // Descomentar cuando esté listo:
        // lobbyClient->selectCar(gameId, selectedCar);
        // ==================================================================
        // Cuando todos los jugadores estén listos, el servidor enviará una señal
        // y deberías:
        //   1. Cerrar la ventana del lobby
        //   2. Iniciar el cliente SDL2 con los datos de la partida
        //   3. Cuando termine el juego, volver a mostrar el lobby
    }
    
    // Por ahora solo actualizamos la lista
    lobbyClient->requestGamesList();
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

QString LobbyWindow::getStatusText(const QString& status) const {
    if (status == "waiting") return "Esperando";
    if (status == "ready") return "Lista";
    if (status == "playing") return "En juego";
    if (status == "full") return "Llena";
    if (status == "finished") return "Finalizada";
    return "Desconocido";
}

QString LobbyWindow::getStatusColor(const QString& status) const {
    if (status == "waiting") return "#4CAF50";   // Verde
    if (status == "ready") return "#FF9800";     // Naranja
    if (status == "playing") return "#F44336";   // Rojo
    if (status == "full") return "#9E9E9E";      // Gris
    if (status == "finished") return "#607D8B";  // Gris azulado
    return "#000000";
}

QWidget* LobbyWindow::createGameItemWidget(const GameInfo& game) {
    ThemeManager& theme = ThemeManager::instance();
    const ColorPalette& palette = theme.getCurrentPalette();
    
    QFrame* frame = new QFrame();
    frame->setFrameShape(QFrame::Box);
    frame->setStyleSheet(theme.gameCardStyle());
    
    QHBoxLayout* layout = new QHBoxLayout(frame);
    layout->setContentsMargins(10, 10, 10, 10);
    
    // === Columna 1: Icono del mapa ===
    QLabel* mapIcon = new QLabel(getMapIcon(game.map));
    mapIcon->setStyleSheet(QString("font-size: 32px; border: none; color: %1;").arg(palette.textPrimary));
    mapIcon->setFixedWidth(50);
    mapIcon->setAlignment(Qt::AlignCenter);
    layout->addWidget(mapIcon);
    
    // === Columna 2: Información principal ===
    QVBoxLayout* infoLayout = new QVBoxLayout();
    
    // Nombre de la partida
    QLabel* nameLabel = new QLabel(game.name);
    nameLabel->setStyleSheet(QString("font-size: 16px; font-weight: bold; border: none; color: %1;").arg(palette.textPrimary));
    infoLayout->addWidget(nameLabel);
    
    // Mapa
    QLabel* mapLabel = new QLabel(QString("Mapa: %1").arg(game.map));
    mapLabel->setStyleSheet(QString("font-size: 12px; color: %1; border: none;").arg(palette.textSecondary));
    infoLayout->addWidget(mapLabel);
    
    layout->addLayout(infoLayout, 1);
    
    // === Columna 3: Jugadores con barra de progreso ===
    QVBoxLayout* playersLayout = new QVBoxLayout();
    
    QLabel* playersLabel = new QLabel(QString("👥 %1/%2").arg(game.currentPlayers).arg(game.maxPlayers));
    playersLabel->setStyleSheet(QString("font-size: 14px; font-weight: bold; border: none; color: %1;").arg(palette.textPrimary));
    playersLayout->addWidget(playersLabel);
    
    // Barra de progreso de jugadores
    QProgressBar* playersBar = new QProgressBar();
    playersBar->setRange(0, game.maxPlayers);
    playersBar->setValue(game.currentPlayers);
    playersBar->setTextVisible(false);
    playersBar->setMaximumHeight(8);
    playersBar->setStyleSheet(theme.progressBarStyle());
    playersLayout->addWidget(playersBar);
    
    layout->addLayout(playersLayout);
    
    // === Columna 4: Estado ===
    QVBoxLayout* statusLayout = new QVBoxLayout();
    
    QString statusText = getStatusText(game.status);
    QString statusColor = getStatusColor(game.status);
    
    QLabel* statusLabel = new QLabel(statusText);
    statusLabel->setStyleSheet(QString(
        "font-size: 12px;"
        "font-weight: bold;"
        "color: white;"
        "background-color: %1;"
        "border: none;"
        "border-radius: 10px;"
        "padding: 5px 15px;"
    ).arg(statusColor));
    statusLabel->setAlignment(Qt::AlignCenter);
    statusLabel->setFixedWidth(100);
    
    statusLayout->addWidget(statusLabel);
    statusLayout->addStretch();
    
    layout->addLayout(statusLayout);
    
    return frame;
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
    
    // Actualizar fondo del widget central
    if (centralWidget) {
        centralWidget->setStyleSheet(QString(
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
