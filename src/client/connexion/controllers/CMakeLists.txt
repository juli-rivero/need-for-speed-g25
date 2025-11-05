target_sources(taller_client
        PRIVATE
        # .cpp files
        searching_controller.cpp
        creating_controller.cpp
        selecting_controller.cpp
        waiting_controller.cpp
        game_controller.cpp
        PUBLIC
        # .h files
        searching_controller.h
        creating_controller.h
        selecting_controller.h
        waiting_controller.h
        game_controller.h
)
