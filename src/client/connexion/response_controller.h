#pragma once

#include <mutex>

#include "client/connexion/controllers/creating_controller.h"
#include "client/connexion/controllers/game_controller.h"
#include "client/connexion/controllers/searching_controller.h"
#include "client/connexion/controllers/selecting_controller.h"
#include "client/connexion/controllers/waiting_controller.h"
#include "client/connexion/receiver.h"

class ResponseController : public ResponseListener {
    SearchingController* searching_controller;
    CreatingController* creating_controller;
    SelectingController* selecting_controller;
    WaitingController* waiting_controller;
    GameController* game_controller;

    std::mutex mtx;

   public:
    ResponseController();

    MAKE_FIXED(ResponseController)

    void control(SearchingWindow&);
    void decontrol(SearchingWindow&);

    void control(CreatingWindow&);
    void decontrol(CreatingWindow&);

    void control(SelectingWindow&);
    void decontrol(SelectingWindow&);

    void control(WaitingWindow&);
    void decontrol(WaitingWindow&);

    void control(Game&);
    void decontrol(Game&);

    ~ResponseController() override;

   private:
    // BROWSER CONTROLLER //
    void recv(const dto_search::JoinResponse&) override;
    void recv(const dto_search::SearchResponse&) override;
    void recv(const dto_search::CreateResponse&) override;

    // LOBBY CONTROLLER //
    void recv(const dto_session::LeaveResponse&) override;
    void recv(const dto_session::StartResponse&) override;

    void recv(const dto::ErrorResponse&) override;

    void decontrol_all();
};
