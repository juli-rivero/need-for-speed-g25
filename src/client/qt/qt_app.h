#pragma once

#include <QApplication>
#include <QMainWindow>
#include <QStackedWidget>

#include "client/connexion/connexion_controller.h"
#include "client/qt/windows/car_selection_dialog.h"
#include "client/qt/windows/create_game_dialog.h"
#include "client/qt/windows/lobby_window.h"
#include "client/qt/windows/waiting_room_widget.h"
#include "common/macros.h"

class QtWindowManager final : public QMainWindow {
    Q_OBJECT

    QStackedWidget stack;

    LobbyWindow searchingRoom;
    CreateGameDialog creatingRoom;
    CarSelectionDialog selectingRoom;
    WaitingRoomWidget waitingRoom;

    ConnexionController& connexion_controller;

   public:
    QtWindowManager(ConnexionController& connexion_controller, bool& quit);
    ~QtWindowManager() override;

    MAKE_FIXED(QtWindowManager)

   private slots:
    void show_searching_room();
    void show_creating_room();
    void show_selecting_room();
    void show_waiting_room();

    void continue_game();

    void applyTheme();
};

class QtApp final {
   public:
    QtApp(ConnexionController& connexion_controller, bool& quit);
};
