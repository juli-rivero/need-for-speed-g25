#include "client/qt/qt_app.h"

#include <QApplication>

#include "client/qt/theme_manager.h"
#include "spdlog/spdlog.h"

QtWindowManager::QtWindowManager(ConnexionController& connexion_controller,
                                 bool& quit)
    : stack(([this] {
          setWindowTitle("Need for Speed - Lobby");
          setMinimumSize(1000, 700);
          spdlog::trace("window configurated");
          return this;
      })()),
      searchingRoom(&stack),
      creatingRoom(&stack),
      selectingRoom(&stack),
      waitingRoom(&stack),
      connexion_controller(connexion_controller) {
    spdlog::trace("qt app created");

    ThemeManager::instance().setTheme(
        ThemeManager::instance().getCurrentTheme());

    setCentralWidget(&stack);

    // Agregarlas al stack
    stack.addWidget(&searchingRoom);
    stack.addWidget(&creatingRoom);
    stack.addWidget(&selectingRoom);
    stack.addWidget(&waitingRoom);

    spdlog::trace("widgets added to stack");

    // Mostrar la primera
    stack.setCurrentWidget(&searchingRoom);

    connect(&searchingRoom, &LobbyWindow::createGameClicked, this,
            &QtWindowManager::show_creating_room);
    connect(&searchingRoom, &LobbyWindow::joinGameClicked, this,
            &QtWindowManager::show_selecting_room);

    connect(&creatingRoom, &CreateGameDialog::submitRequested, this,
            &QtWindowManager::show_selecting_room);
    connect(&creatingRoom, &CreateGameDialog::cancelRequested, this,
            &QtWindowManager::show_searching_room);

    connect(&selectingRoom, &CarSelectionDialog::confirmRequested, this,
            &QtWindowManager::show_waiting_room);
    connect(&selectingRoom, &CarSelectionDialog::cancelRequested, this,
            &QtWindowManager::show_searching_room);

    connect(&waitingRoom, &WaitingRoomWidget::readyStateChanged, this,
            &QtWindowManager::continue_game);
    connect(&waitingRoom, &WaitingRoomWidget::leaveGameRequested, this,
            &QtWindowManager::show_searching_room);

    connect(&ThemeManager::instance(), &ThemeManager::themeChanged, this,
            &QtWindowManager::applyTheme);
    applyTheme();

    spdlog::trace("signals connected");

    show();

    spdlog::trace("showing window");

    spdlog::trace("exec finished");

    quit = false;  // TODO(juli): borrar
}

QtWindowManager::~QtWindowManager() {}

void QtWindowManager::show_searching_room() {
    setWindowTitle("Need for Speed - Buscador de Salas");
    stack.setCurrentWidget(&searchingRoom);
}
void QtWindowManager::show_creating_room() {
    setWindowTitle("Need for Speed - Crear Partida");
    stack.setCurrentWidget(&creatingRoom);
}
void QtWindowManager::show_selecting_room() {
    setWindowTitle("Need for Speed - Seleccionar Auto");
    stack.setCurrentWidget(&selectingRoom);
}
void QtWindowManager::show_waiting_room() {
    setWindowTitle("Need for Speed - Sala de Espera");
    stack.setCurrentWidget(&waitingRoom);
}

void QtWindowManager::continue_game() { QCoreApplication::quit(); }

void QtWindowManager::applyTheme() {
    const ThemeManager& theme = ThemeManager::instance();
    const ColorPalette& palette = theme.getCurrentPalette();
    stack.setStyleSheet(QString("QWidget {"
                                "    background-color: %1;"
                                "}")
                            .arg(palette.cardBackgroundHover));
}

QtApp::QtApp(ConnexionController& connexion_controller, bool& quit) {
    int argc = 0;
    spdlog::trace("creando qpp");
    QApplication app(argc, nullptr);
    spdlog::trace("creando main");
    QtWindowManager main(connexion_controller, quit);
    spdlog::trace("exec");
    app.exec();
}
