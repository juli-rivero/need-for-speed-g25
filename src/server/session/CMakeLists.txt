target_sources(taller_server
        PRIVATE
        game.cpp
        session.cpp
        PUBLIC
        game.h
        session.h
)

add_subdirectory(session/model)
add_subdirectory(session/physics)
add_subdirectory(session/logic)