#include "client/connexion/controllers/creating_controller.h"

#include <client/qt/windows/creating_window.h>

CreatingController::CreatingController(CreatingWindow& creating_window)
    : creating_window(creating_window) {}
void CreatingController::recv(const dto_search::CreateResponse&) const {
    creating_window.sessionCreated();
}
