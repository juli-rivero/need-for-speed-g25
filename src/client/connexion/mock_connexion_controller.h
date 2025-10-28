#pragma once
#include "client/connexion/connexion_controller.h"
#include "common/dto/dto_lobby.h"
#include "common/dto/dto_session.h"

class MockConnexionController final : public ResponseController {
    MockConnexionController() = default;
    MAKE_FIXED(MockConnexionController)

    void send(const dto_session::SearchRequest&);
    void send(const dto_session::JoinRequest&);
    void send(const dto_session::LeaveRequest&);
    void send(const dto_lobby::StartRequest&);
};
