#pragma once

#include "common/dto/dto.h"
#include "common/dto/dto_session.h"
#include "common/macros.h"

class SearchingWindow;

class SearchingController final {
    SearchingWindow& searching_window;

   public:
    explicit SearchingController(SearchingWindow& searching_window);

    MAKE_FIXED(SearchingController)

    void recv(const dto_session::JoinResponse&);
    void recv(const dto_session::SearchResponse&);
    void recv(const dto::ErrorResponse&);
};
