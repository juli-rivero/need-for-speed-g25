#pragma once

#include <mutex>

#include "client/connexion/controllers/creating_controller.h"
#include "client/connexion/controllers/game_controller.h"
#include "client/connexion/controllers/searching_controller.h"
#include "client/connexion/controllers/selecting_controller.h"
#include "client/connexion/controllers/waiting_controller.h"
#include "client/connexion/receiver.h"

struct IResponseController {
    virtual void control(SearchingWindow&) = 0;
    virtual void decontrol(SearchingWindow&) = 0;

    virtual void control(CreatingWindow&) = 0;
    virtual void decontrol(CreatingWindow&) = 0;

    virtual void control(SelectingWindow&) = 0;
    virtual void decontrol(SelectingWindow&) = 0;

    virtual void control(WaitingWindow&) = 0;
    virtual void decontrol(WaitingWindow&) = 0;

    virtual void control(Game&) = 0;
    virtual void decontrol(Game&) = 0;

    virtual ~IResponseController() = default;
};

class ResponseController : public IResponseListener,
                           virtual public IResponseController {
    SearchingController* searching_controller;
    CreatingController* creating_controller;
    SelectingController* selecting_controller;
    WaitingController* waiting_controller;
    GameController* game_controller;

    std::mutex mtx;

   public:
    ResponseController();

    MAKE_FIXED(ResponseController)

    void control(SearchingWindow&) override;
    void decontrol(SearchingWindow&) override;

    void control(CreatingWindow&) override;
    void decontrol(CreatingWindow&) override;

    void control(SelectingWindow&) override;
    void decontrol(SelectingWindow&) override;

    void control(WaitingWindow&) override;
    void decontrol(WaitingWindow&) override;

    void control(Game&) override;
    void decontrol(Game&) override;

    // BROWSER CONTROLLER //
    void recv(const dto_session::LeaveResponse&) override;
    void recv(const dto_session::JoinResponse&) override;
    void recv(const dto_session::SearchResponse&) override;

    // LOBBY CONTROLLER //
    void recv(const dto_lobby::StartResponse&) override;

    void recv(const dto::ErrorResponse&) override;

    ~ResponseController() override;

   private:
    void decontrol_all();
};
