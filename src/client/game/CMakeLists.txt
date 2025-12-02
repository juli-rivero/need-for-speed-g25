target_sources(taller_client
        PRIVATE
        # .cpp files
        sdl_app.cpp
        game.cpp
        screen.cpp
        sound.cpp
        assets.cpp
        bounding_box.cpp
        car.cpp
        PUBLIC
        # .h files
        sdl_app.h
        game.h
        screen.h
        sound.h
        assets.h
        bounding_box.h
        car.h
)
