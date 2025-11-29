target_sources(taller_client
        PRIVATE
        # .cpp files
        game.cpp
        sdl_app.cpp
        assets.cpp
        map.cpp
        car.cpp
        screen.cpp
        sound.cpp
        PUBLIC
        # .h files
        sdl_app.h
        classes.h    # TODO(franco): TEMPORAL, SEPARAR.
        assets.h
        map.h
)
