#pragma once

#include "client/qt/windows/waiting_room_widget.h"
#include "common/dto/dto_lobby.h"
#include "common/macros.h"

class LobbyController final {
    WaitingRoomWidget& waiting_room;

   public:
    explicit LobbyController(WaitingRoomWidget& waiting_room);

    MAKE_FIXED(LobbyController)

    void on(const dto_lobby::StartResponse&);
};
