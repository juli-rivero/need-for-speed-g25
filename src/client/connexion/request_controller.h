#pragma once
#include <string>

#include "client/connexion/sender.h"

struct IRequestController {
    virtual void request_all_sessions() = 0;
    virtual void request_join_session(const std::string& session_id) = 0;
    virtual void request_leave_current_session() = 0;
    virtual void request_start_game() = 0;
    virtual ~IRequestController() = default;
};

class RequestController : virtual public IRequestController {
    Sender& sender;

   public:
    explicit RequestController(Sender& sender);
    void request_all_sessions() override;
    void request_join_session(const std::string& session_id) override;
    void request_leave_current_session() override;
    void request_start_game() override;
    ~RequestController() override = default;
};
