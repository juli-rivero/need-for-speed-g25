target_sources(taller_client
        PRIVATE
        # .cpp files
        game.cpp
        sdl_app.cpp
        assets.cpp
        car.cpp
        screen.cpp
        sound.cpp
        bounding_box.cpp
        PUBLIC
        # .h files
        sdl_app.h
        classes.h    # TODO(franco): TEMPORAL, SEPARAR.
        assets.h
        bounding_box.h
        car.h
)
