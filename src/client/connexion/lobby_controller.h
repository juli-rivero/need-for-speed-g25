#pragma once

#include "client/qt/lobby.h"
#include "common/dto/dto_lobby.h"

class LobbyController final {
    Lobby& lobby;

   public:
    explicit LobbyController(Lobby& lobby);

    MAKE_FIXED(LobbyController)

    void on(const dto_lobby::StartResponse&);
};
