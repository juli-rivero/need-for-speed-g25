add_subdirectory(game)
add_subdirectory(qt)
add_subdirectory(connexion)

target_sources(taller_client
        PRIVATE
        main.cpp
        args_parser.cpp
        app.cpp
        PUBLIC
        args_parser.cpp
        app.h
)
