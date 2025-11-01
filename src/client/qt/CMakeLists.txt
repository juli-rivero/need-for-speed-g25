target_sources(taller_client
    PRIVATE
    # .cpp files
    main.cpp
    lobby_window.cpp
    create_game_dialog.cpp
    car_selection_dialog.cpp
    waiting_room_widget.cpp
    game_card_widget.cpp
    lobby_client.cpp
    theme_manager.cpp
    PUBLIC
    # .h files
    lobby_window.h
    create_game_dialog.h
    car_selection_dialog.h
    waiting_room_widget.h
    game_card_widget.h
    lobby_client.h
    theme_manager.h
    )
