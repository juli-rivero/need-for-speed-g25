add_subdirectory(game)
add_subdirectory(qt)
add_subdirectory(connexion)

target_sources(taller_client
        PRIVATE
        main.cpp
        app.cpp
        PUBLIC
        app.h
)
