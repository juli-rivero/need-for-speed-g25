add_subdirectory(client_handler)
add_subdirectory(session)

target_sources(taller_server
        PRIVATE
        main.cpp
        PRIVATE
        acceptor.cpp
        clients_manager.cpp
        server.cpp
        sessions_monitor.cpp
        PUBLIC
        acceptor.h
        clients_manager.h
        server.h
        sessions_monitor.h
)
