add_subdirectory(windows)
add_subdirectory(ui)

target_sources(taller_client
    PRIVATE
    # .cpp files
    game_card_widget.cpp
    theme_manager.cpp
    qt_app.cpp
    PUBLIC
    # .h files
    game_card_widget.h
    theme_manager.h
    qt_app.h
    )
