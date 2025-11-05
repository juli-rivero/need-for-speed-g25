#pragma once

#include "client/connexion/receiver.h"
#include "client/connexion/request_controller.h"
#include "client/connexion/response_controller.h"
#include "client/connexion/sender.h"

struct IConnexionController : virtual IRequestController,
                              virtual IResponseController {};

class ConnexionController final : public IConnexionController,
                                  public ResponseController,
                                  public RequestController {
    Protocol protocol;
    Receiver receiver;
    Sender sender;

   public:
    explicit ConnexionController(Protocol&& protocol);

    MAKE_UNCOPIABLE(ConnexionController)

    ~ConnexionController() override;

    using RequestController::request_all_sessions;
    using RequestController::request_join_session;
    using RequestController::request_leave_current_session;
    using RequestController::request_start_game;
    using ResponseController::control;
    using ResponseController::decontrol;

   private:
    [[nodiscard]] bool is_alive() const;
};
