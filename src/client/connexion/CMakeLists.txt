target_sources(taller_client
        PRIVATE
        # .cpp files
        connexion_controller.cpp
        receiver.cpp
        sender.cpp
        response_controller.cpp
        browser_controller.cpp
        lobby_controller.cpp
        game_controller.cpp
        PUBLIC
        # .h files
        connexion_controller.h
        receiver.h
        sender.h
        response_controller.h
        browser_controller.h
        lobby_controller.h
        game_controller.h
)
