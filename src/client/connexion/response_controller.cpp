#include "client/connexion/response_controller.h"

#include "spdlog/spdlog.h"

ResponseController::ResponseController()
    : searching_controller(nullptr),
      creating_controller(nullptr),
      selecting_controller(nullptr),
      waiting_controller(nullptr),
      game_controller(nullptr) {}

#define DEFINE_CONTROL(WindowType, controller, ControllerType) \
    void ResponseController::control(WindowType& window) {     \
        std::lock_guard lock(mtx);                             \
        delete controller;                                     \
        controller = new ControllerType(window);               \
    }
#define DEFINE_UNCONTROL(WindowType, controller)      \
    void ResponseController::decontrol(WindowType&) { \
        std::lock_guard lock(mtx);                    \
        delete controller;                            \
        controller = nullptr;                         \
    }
#define DEFINE_CONTROL_UNCONTROL(WindowType, controller, ControllerType) \
    DEFINE_CONTROL(WindowType, controller, ControllerType)               \
    DEFINE_UNCONTROL(WindowType, controller)

DEFINE_CONTROL_UNCONTROL(SearchingWindow, searching_controller,
                         SearchingController)
DEFINE_CONTROL_UNCONTROL(CreatingWindow, creating_controller,
                         CreatingController)
DEFINE_CONTROL_UNCONTROL(SelectingWindow, selecting_controller,
                         SelectingController)
DEFINE_CONTROL_UNCONTROL(WaitingWindow, waiting_controller, WaitingController)
DEFINE_CONTROL_UNCONTROL(Game, game_controller, GameController)

void ResponseController::decontrol_all() {
    std::lock_guard lock(mtx);
    delete searching_controller;
    searching_controller = nullptr;
    delete creating_controller;
    creating_controller = nullptr;
    delete selecting_controller;
    selecting_controller = nullptr;
    delete waiting_controller;
    waiting_controller = nullptr;
    delete game_controller;
    game_controller = nullptr;
}

ResponseController::~ResponseController() { decontrol_all(); }

void ResponseController::recv(const dto_session::LeaveResponse&) {}

void ResponseController::recv(const dto_session::JoinResponse& r) {
    std::lock_guard lock(mtx);
    if (searching_controller) searching_controller->recv(r);
}
void ResponseController::recv(const dto_session::SearchResponse& r) {
    std::lock_guard lock(mtx);
    if (searching_controller) searching_controller->recv(r);
}

void ResponseController::recv(const dto_lobby::StartResponse& r) {
    std::lock_guard lock(mtx);
    if (waiting_controller) waiting_controller->recv(r);
}
void ResponseController::recv(const dto::ErrorResponse&) {}
