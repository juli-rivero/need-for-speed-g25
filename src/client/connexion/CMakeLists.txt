add_subdirectory(controllers)

target_sources(taller_client
        PRIVATE
        # .cpp files
        mock_connexion_controller.cpp
        connexion_controller.cpp
        receiver.cpp
        sender.cpp
        response_controller.cpp
        request_controller.cpp
        PUBLIC
        # .h files
        mock_connexion_controller.h
        connexion_controller.h
        receiver.h
        sender.h
        response_controller.h
        request_controller.h
)
