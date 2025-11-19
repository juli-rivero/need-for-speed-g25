#pragma once

#include <QApplication>
#include <QMainWindow>
#include <QStackedWidget>

#include "client/connexion/connexion.h"
#include "client/qt/windows/creating_window.h"
#include "client/qt/windows/searching_window.h"
#include "client/qt/windows/selecting_window.h"
#include "client/qt/windows/waiting_window.h"
#include "common/macros.h"

class QtWindowManager final : public QMainWindow {
    Q_OBJECT

    QStackedWidget stack;

    SearchingWindow searchingWindow;
    CreatingWindow creatingWindow;
    SelectingWindow selectingWindow;
    WaitingWindow waitingWindow;

   public:
    QtWindowManager(Connexion& connexion, bool& quit);

    MAKE_FIXED(QtWindowManager)

   private slots:
    void show_searching_window();
    void show_creating_window();
    void show_selecting_window();
    void show_waiting_window();

    void continue_game();

    void applyTheme();
};

class QtApp final {
   public:
    QtApp(Connexion& connexion, bool& quit);
};
