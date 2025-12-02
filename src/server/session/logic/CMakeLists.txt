target_sources(taller_server
        PRIVATE
        GameSessionFacade.cpp
        PUBLIC
        GameSessionFacade.h
        types.h

)
add_subdirectory(match)
add_subdirectory(race)
