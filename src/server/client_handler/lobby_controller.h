#pragma once

#include "common/dto/dto_lobby.h"
#include "server/client_handler/sender.h"
#include "server/session/session.h"

struct ILobbyEvents {
    virtual void on_start_game(Game& game) = 0;
    virtual ~ILobbyEvents() = default;
};

class LobbyController final : SessionListener {
    const int client_id;
    Sender& sender;
    ILobbyEvents& dispatcher;

   public:
    LobbyController(Session& session, int client_id, Sender& sender,
                    ILobbyEvents& handler);

    MAKE_FIXED(LobbyController)

    void on(const dto_lobby::StartRequest&);

    void on_start_game(Game& game) override;
};
