#pragma once

#include "common/dto/dto_session.h"
#include "common/macros.h"

class WaitingWindow;

class WaitingController final {
    WaitingWindow& waiting_window;

   public:
    explicit WaitingController(WaitingWindow& waiting_window);

    MAKE_FIXED(WaitingController)

    void recv(const dto_session::StartResponse&);
};
