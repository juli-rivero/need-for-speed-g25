#pragma once

#include "client/qt/windows/lobby_window.h"
#include "common/dto/dto.h"
#include "common/dto/dto_session.h"
#include "common/macros.h"

class BrowserController final {
    LobbyWindow& searching_room;

   public:
    explicit BrowserController(LobbyWindow& searching_room);

    MAKE_FIXED(BrowserController)

    void on(const dto_session::LeaveResponse&);
    void on(const dto_session::JoinResponse&);
    void on(const dto_session::SearchResponse&);
    void on(const dto::ErrorResponse&);
};
