if (TALLER_CLIENT OR TALLER_EDITOR)
    add_custom_target(copy_assets
            COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/assets ${CMAKE_CURRENT_BINARY_DIR}/assets
    )

    add_dependencies(taller_client copy_assets)
    add_dependencies(taller_editor copy_assets)
endif ()

if (TALLER_SERVER OR TALLER_EDITOR)
    add_custom_target(copy_maps
            COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/maps ${CMAKE_CURRENT_BINARY_DIR}/maps
    )

    add_dependencies(taller_server copy_maps)
    add_dependencies(taller_editor copy_maps)
endif ()

if (TALLER_SERVER)
    add_custom_target(copy_config
            COMMAND ${CMAKE_COMMAND} -E copy
            ${CMAKE_SOURCE_DIR}/config.yaml
            ${CMAKE_CURRENT_BINARY_DIR}/config.yaml
    )

    add_dependencies(taller_server copy_config)
endif ()
