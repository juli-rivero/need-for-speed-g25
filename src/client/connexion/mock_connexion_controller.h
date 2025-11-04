#pragma once

#include <string>

#include "client/connexion/connexion_controller.h"
#include "common/macros.h"

class MockConnexionController final : public IConnexionController,
                                      public ResponseController {
   public:
    MockConnexionController() {}
    ~MockConnexionController() override = default;
    MAKE_FIXED(MockConnexionController)

    using ResponseController::control;
    using ResponseController::decontrol;

    void request_all_sessions() override;
    void request_join_session(const std::string& session_id) override;
    void request_leave_current_session() override;
    void request_start_game() override;
};
