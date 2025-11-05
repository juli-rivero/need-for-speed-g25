#pragma once

class SelectingWindow;

class SelectingController {
    SelectingWindow& selecting_window;

   public:
    explicit SelectingController(SelectingWindow& selecting_window);
};
