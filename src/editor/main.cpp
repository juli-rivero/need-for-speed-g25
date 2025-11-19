#include <QApplication>

#include "editor/editor_window.h"
#include "editor/start_dialog.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    StartDialog startDialog;
    if (startDialog.exec() == QDialog::Rejected) {
        return 0;  // Usuario canceló
    }

    // Crear ventana principal según el modo seleccionado
    EditorWindow::EditorMode mode;
    if (startDialog.getSelectedMode() == StartDialog::EditRace) {
        mode = EditorWindow::EditRaceMode;
    } else {
        mode = EditorWindow::CreateMapMode;
    }

    EditorWindow window(mode);
    window.show();

    return app.exec();
}
