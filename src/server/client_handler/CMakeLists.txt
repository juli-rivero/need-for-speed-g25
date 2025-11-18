add_subdirectory(controllers)

target_sources(taller_server
        PRIVATE
        client_handler.cpp
        receiver.cpp
        sender.cpp
        controller.cpp
        PUBLIC
        client_handler.h
        receiver.h
        sender.h
        controller.h
)
