target_sources(taller_editor
    PRIVATE
    # .cpp files
    main.cpp
    editor_window.cpp
    start_dialog.cpp
    map_canvas.cpp
    yaml_handler.cpp
    checkpoint_item.cpp
    hint_item.cpp
    building_item.cpp

    PUBLIC
    # .h files
    editor_window.h
    start_dialog.h
    map_canvas.h
    yaml_handler.h
    map_item.h
    checkpoint_item.h
    hint_item.h
    building_item.h
)

# Buscar yaml-cpp usando pkg-config
find_package(PkgConfig REQUIRED)
pkg_check_modules(YAML_CPP REQUIRED yaml-cpp)

# Agregar directorios de include de yaml-cpp
target_include_directories(taller_editor PRIVATE ${YAML_CPP_INCLUDE_DIRS})

# Enlazar con yaml-cpp
target_link_libraries(taller_editor ${YAML_CPP_LIBRARIES})
