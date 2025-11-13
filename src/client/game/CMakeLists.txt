target_sources(taller_client
        PRIVATE
        # .cpp files
        game.cpp
        input_handler.cpp
        sdl_app.cpp
        assets.cpp
        PUBLIC
        # .h files
        game.h
        input_handler.h
        sdl_app.h
        assets.h
)
