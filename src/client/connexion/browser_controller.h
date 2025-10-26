#pragma once

#include "client/qt/browser.h"
#include "common/dto/dto_session.h"

class BrowserController final {
    Browser& browser;

   public:
    explicit BrowserController(Browser& browser);

    MAKE_FIXED(BrowserController)

    void on(const dto_session::LeaveResponse&);
    void on(const dto_session::JoinResponse&);
    void on(const dto_session::SearchResponse&);
};
