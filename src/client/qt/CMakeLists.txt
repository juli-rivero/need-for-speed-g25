add_subdirectory(windows)
add_subdirectory(ui)

target_sources(taller_client
    PRIVATE
    # .cpp files
    game_card_widget.cpp
    lobby_client.cpp
    theme_manager.cpp
    qt_app.cpp
    PUBLIC
    # .h files
    game_card_widget.h
    lobby_client.h
    theme_manager.h
    qt_app.h
    )
