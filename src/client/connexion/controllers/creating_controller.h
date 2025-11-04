#pragma once

class CreatingWindow;

class CreatingController {
    CreatingWindow& creating_window;

   public:
    explicit CreatingController(CreatingWindow& creating_window);
};
