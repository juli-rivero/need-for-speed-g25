add_subdirectory(controllers)
add_subdirectory(requesters)

target_sources(taller_client
        PRIVATE
        # .cpp files
        connexion.cpp
        receiver.cpp
        sender.cpp
        response_controller.cpp
        PUBLIC
        # .h files
        connexion.h
        receiver.h
        sender.h
        response_controller.h
)
