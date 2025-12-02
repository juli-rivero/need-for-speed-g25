target_sources(taller_editor
    PRIVATE
    # .cpp files
    main.cpp
    editor_window.cpp
    start_dialog.cpp
    map_canvas.cpp
    yaml_handler.cpp
    checkpoint_item.cpp
    building_item.cpp
    sensor_item.cpp
    overpass_item.cpp

    PUBLIC
    # .h files
    editor_window.h
    sensor_item.h
    start_dialog.h
    map_canvas.h
    yaml_handler.h
    map_item.h
    checkpoint_item.h
    building_item.h
    overpass_item.h
)
