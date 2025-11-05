#pragma once

#include "common/dto/dto_lobby.h"
#include "common/macros.h"

class WaitingWindow;

class WaitingController final {
    WaitingWindow& waiting_window;

   public:
    explicit WaitingController(WaitingWindow& waiting_window);

    MAKE_FIXED(WaitingController)

    void recv(const dto_lobby::StartResponse&);
};
