if (TALLER_CLIENT OR TALLER_EDITOR)
    add_custom_target(copy_assets
            COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/assets ${CMAKE_CURRENT_BINARY_DIR}/assets
    )

    add_dependencies(taller_client copy_assets)
    add_dependencies(taller_editor copy_assets)
endif ()
