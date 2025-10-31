target_sources(taller_server
    PRIVATE
    main.cpp
)
add_subdirectory(model)
add_subdirectory(physics)
add_subdirectory(logic)
add_subdirectory(config)
