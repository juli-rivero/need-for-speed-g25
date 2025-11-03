target_sources(taller_client
        PRIVATE
        # .cpp files
        lobby_window.cpp
        create_game_dialog.cpp
        car_selection_dialog.cpp
        waiting_room_widget.cpp
        PUBLIC
        # .h files
        lobby_window.h
        create_game_dialog.h
        car_selection_dialog.h
        waiting_room_widget.h
)
