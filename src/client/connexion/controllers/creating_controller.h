#pragma once
#include <common/dto/dto_search.h>

class CreatingWindow;

class CreatingController {
    CreatingWindow& creating_window;

   public:
    explicit CreatingController(CreatingWindow& creating_window);

    void recv(const dto_search::CreateResponse&) const;
};
