#include "client/qt/qt_app.h"

#include <QApplication>

#include "client/qt/theme_manager.h"
#include "spdlog/spdlog.h"

QtWindowManager::QtWindowManager(Connexion& connexion, bool& quit,
                                 GameSetUp& setup)
    : stack(([this] {
          setWindowTitle("Need for Speed - Lobby");
          setMinimumSize(1000, 700);
          spdlog::trace("window configurated");
          return this;
      })()),
      searchingWindow(&stack, connexion),
      creatingWindow(&stack, connexion),
      selectingWindow(&stack, connexion),
      waitingWindow(&stack, connexion),
      setup(setup) {
    setCentralWidget(&stack);

    // Agregarlas al stack
    stack.addWidget(&searchingWindow);
    stack.addWidget(&creatingWindow);
    stack.addWidget(&selectingWindow);
    stack.addWidget(&waitingWindow);

    spdlog::trace("widgets added to stack");

    // Mostrar la primera
    stack.setCurrentWidget(&searchingWindow);

    connect(&searchingWindow, &SearchingWindow::createGameClicked, this,
            &QtWindowManager::show_creating_window);
    connect(&searchingWindow, &SearchingWindow::joinGameClicked, this,
            &QtWindowManager::show_selecting_window);

    connect(&creatingWindow, &CreatingWindow::sessionCreated, this,
            &QtWindowManager::show_selecting_window);
    connect(&creatingWindow, &CreatingWindow::createCanceled, this,
            &QtWindowManager::show_searching_window);

    connect(&selectingWindow, &SelectingWindow::confirmRequested, this,
            &QtWindowManager::show_waiting_window);
    connect(&selectingWindow, &SelectingWindow::cancelRequested, this,
            &QtWindowManager::show_searching_window);

    connect(&waitingWindow, &WaitingWindow::startGameRequested, this,
            &QtWindowManager::continue_game);
    connect(&waitingWindow, &WaitingWindow::leaveGameRequested, this,
            &QtWindowManager::show_searching_window);

    connect(&ThemeManager::instance(), &ThemeManager::themeChanged, this,
            &QtWindowManager::applyTheme);
    applyTheme();

    spdlog::trace("signals connected");

    show();

    spdlog::trace("showing window");

    quit = false;  // TODO(juli): borrar
}

void QtWindowManager::show_searching_window() {
    setWindowTitle("Need for Speed - Buscador de Salas");
    stack.setCurrentWidget(&searchingWindow);
}
void QtWindowManager::show_creating_window() {
    setWindowTitle("Need for Speed - Crear Partida");
    stack.setCurrentWidget(&creatingWindow);
}
void QtWindowManager::show_selecting_window() {
    setWindowTitle("Need for Speed - Seleccionar Auto");
    stack.setCurrentWidget(&selectingWindow);
}
void QtWindowManager::show_waiting_window() {
    setWindowTitle("Need for Speed - Sala de Espera");
    stack.setCurrentWidget(&waitingWindow);
}
void QtWindowManager::continue_game(const CityInfo& city_info,
                                    const RaceInfo& race_info) {
    setup.city_info = city_info;
    setup.race_info = race_info;
    QCoreApplication::quit();
}

void QtWindowManager::applyTheme() {
    const ThemeManager& theme = ThemeManager::instance();
    const ColorPalette& palette = theme.getCurrentPalette();
    stack.setStyleSheet(QString("QWidget {"
                                "    background-color: %1;"
                                "}")
                            .arg(palette.cardBackgroundHover));
}

QtApp::QtApp(Connexion& connexion, bool& quit, GameSetUp& setup) {
    int argc = 0;

    spdlog::trace("creando qt app");
    QApplication app(argc, nullptr);
    spdlog::trace("creando main");
    QtWindowManager main(connexion, quit, setup);
    spdlog::trace("exec");
    app.exec();
}
