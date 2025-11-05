target_sources(taller_server
    PRIVATE
        main.cpp
)
add_subdirectory(server/session)
add_subdirectory(server/config)
