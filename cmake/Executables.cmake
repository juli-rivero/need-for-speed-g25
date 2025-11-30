if (TALLER_CLIENT)
    add_executable(taller_client)

    # Make them depend on taller_common lib
    add_dependencies(taller_client taller_common SDL2pp::SDL2pp Qt6::Core Qt6::Widgets cxxopts spdlog)

    # Add source files and enable warnings
    # HEY!! TODO XXX: add more subfolders here
    add_subdirectory(src/client)

    set_project_warnings(taller_client ${TALLER_MAKE_WARNINGS_AS_ERRORS} FALSE)

    # add libSDL2pp's headers
    target_include_directories(taller_client PRIVATE ${libSDL2pp_SOURCE_DIR})

    # Link the taller_common lib target
    target_link_libraries(taller_client taller_common SDL2pp::SDL2pp Qt6::Core Qt6::Widgets cxxopts spdlog)
endif ()

if (TALLER_SERVER)
    add_executable(taller_server)

    # Make them depend on taller_common lib
    add_dependencies(taller_server
            taller_common
            SDL2pp::SDL2pp
            box2d
            yaml-cpp
            cxxopts
            spdlog
    )

    # Add source files and enable warnings
    # HEY!! TODO XXX: add more subfolders here
    add_subdirectory(src/server)

    set_project_warnings(taller_server ${TALLER_MAKE_WARNINGS_AS_ERRORS} FALSE)

    target_include_directories(taller_server PRIVATE ${libSDL2pp_SOURCE_DIR} box2d)

    target_compile_definitions(taller_server PRIVATE OFFLINE=$<BOOL:${OFFLINE}>)

    # Link the taller_common lib target
    target_link_libraries(taller_server PRIVATE
            taller_common
            SDL2pp::SDL2pp
            box2d
            yaml-cpp
            cxxopts
            spdlog
    )

endif ()

if (TALLER_EDITOR)
    add_executable(taller_editor)

    # Make them depend on taller_common lib
    add_dependencies(taller_editor taller_common Qt6::Core Qt6::Widgets yaml-cpp)

    target_include_directories(taller_editor PRIVATE yaml-cpp)

    # Add source files and enable warnings
    add_subdirectory(src/editor)

    set_project_warnings(taller_editor ${TALLER_MAKE_WARNINGS_AS_ERRORS} FALSE)

    # Link the taller_common lib target
    target_link_libraries(taller_editor taller_common Qt6::Core Qt6::Widgets yaml-cpp)
endif ()


if (TALLER_TESTS)
    # target for testing
    # HEY!! TODO XXX: Change the name of the executable to the one you like!
    add_executable(taller_tests)

    # HEY!! TODO XXX: I'm assuming that the code under testing is in taller_common
    # Change this if you have to.
    add_dependencies(taller_tests taller_common)

    # Say where to find the xoz headers
    # HEY!! TODO XXX: add more subfolders here
    target_include_directories(taller_tests PUBLIC .)

    # Add the source files to the taller_tests target
    # HEY!! TODO XXX: add more subfolders here
    add_subdirectory(src/tests/)

    # Set the compiler warnings to the tests target (but be a little lesser strict)
    set_project_warnings(taller_tests ${TALLER_MAKE_WARNINGS_AS_ERRORS} TRUE)

    # Say where googletest headers can be found
    target_include_directories(taller_tests
            PUBLIC
            ${gtest_SOURCE_DIR}/include
            ${gmock_SOURCE_DIR}/include
    )

    # HEY!! TODO XXX: I'm assuming that the code under testing is in taller_common
    # Change this if you have to.
    target_link_libraries(taller_tests
            taller_common
            GTest::gtest_main
    )
endif ()
