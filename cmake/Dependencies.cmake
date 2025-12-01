#
# Install libSDL2 and libSDL2pp (for client or editor), it needs dependencies:
# In xubuntu 24.04 these are:
#
# sudo apt-get install libopus-dev libopusfile-dev libxmp-dev libfluidsynth-dev fluidsynth libwavpack1 libwavpack-dev libfreetype-dev wavpack
#

# Install command arguments parser and logging library


if (TALLER_CLIENT OR TALLER_SERVER)
    include(FetchContent)
    FetchContent_Declare(
            cxxopts
            GIT_REPOSITORY https://github.com/jarro2783/cxxopts.git
            GIT_TAG v3.2.1
            OVERRIDE_FIND_PACKAGE
    )
    FetchContent_Declare(
            spdlog
            GIT_REPOSITORY https://github.com/gabime/spdlog.git
            GIT_TAG v1.16.0
            OVERRIDE_FIND_PACKAGE
    )
    FetchContent_MakeAvailable(cxxopts spdlog)
endif ()

# Install libSDL2 and libSDL2pp (for client or mock client in server)
if (TALLER_CLIENT OR TALLER_SERVER)
    set(SDL2PP_WITH_IMAGE yes)
    set(SDL2PP_WITH_MIXER yes)
    set(SDL2PP_WITH_TTF yes)

    include(FetchContent)
    FetchContent_Declare(
            SDL2
            GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
            GIT_TAG release-2.32.10
            GIT_SHALLOW TRUE
            GIT_PROGRESS TRUE
            OVERRIDE_FIND_PACKAGE
    )

    FetchContent_Declare(
            SDL2_image
            GIT_REPOSITORY https://github.com/libsdl-org/SDL_image.git
            GIT_TAG release-2.8.2
            GIT_SHALLOW TRUE
            GIT_PROGRESS TRUE
            OVERRIDE_FIND_PACKAGE
    )
    set(SDL2MIXER_MIDI OFF CACHE BOOL "" FORCE)
    set(SDL2MIXER_OPUS OFF CACHE BOOL "" FORCE)
    set(SDL2MIXER_VORBIS OFF CACHE BOOL "" FORCE)
    set(SDL2MIXER_WAVPACK OFF CACHE BOOL "" FORCE)
    FetchContent_Declare(
            SDL2_mixer
            GIT_REPOSITORY https://github.com/libsdl-org/SDL_mixer.git
            GIT_TAG release-2.8.0
            GIT_SHALLOW TRUE
            GIT_PROGRESS TRUE
            OVERRIDE_FIND_PACKAGE
    )

    FetchContent_Declare(
            SDL2_ttf
            GIT_REPOSITORY https://github.com/libsdl-org/SDL_ttf.git
            GIT_TAG release-2.22.0
            GIT_SHALLOW TRUE
            GIT_PROGRESS TRUE
            OVERRIDE_FIND_PACKAGE
    )

    set(SDL2PP_WITH_TTF ON CACHE BOOL "" FORCE)
    set(SDL2PP_WITH_MIXER ON CACHE BOOL "" FORCE)

    FetchContent_Declare(
            libSDL2pp
            OVERRIDE_FIND_PACKAGE
            URL https://github.com/libSDL2pp/libSDL2pp/archive/cc198c9a5657048bee67ece82de620b2d5661084.zip
    )

    FetchContent_MakeAvailable(SDL2 SDL2_image SDL2_mixer SDL2_ttf libSDL2pp)
endif ()

if (TALLER_SERVER)
    include(FetchContent)
    FetchContent_Declare(
            box2d
            GIT_REPOSITORY https://github.com/erincatto/box2d.git
            GIT_TAG v3.1.1
            GIT_SHALLOW TRUE
            GIT_PROGRESS TRUE
            OVERRIDE_FIND_PACKAGE
    )
    FetchContent_MakeAvailable(box2d)
endif ()

if (TALLER_SERVER OR TALLER_EDITOR)
    include(FetchContent)
    FetchContent_Declare(
            yaml
            GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
            GIT_TAG 0.8.0
            GIT_SHALLOW TRUE
            GIT_PROGRESS TRUE
            OVERRIDE_FIND_PACKAGE
    )
    FetchContent_MakeAvailable(yaml)
endif ()

if (TALLER_CLIENT OR TALLER_EDITOR)
    # TODO: hacer un installer con las dependencias de qt:
    # En ubuntu:
    # sudo apt update
    # sudo apt install qt6-base-dev qt6-tools-dev cmake build-essential -y

    # Buscar Qt6 (componentes necesarios para GUI)
    find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

    # Habilitar herramientas automáticas de Qt
    set(CMAKE_AUTOMOC ON)  # Meta-Object Compiler (para signals/slots)
    set(CMAKE_AUTOUIC ON)  # User Interface Compiler (para archivos .ui)
    set(CMAKE_AUTORCC ON)  # Resource Compiler (para archivos .qrc)

    message(STATUS "Qt6 encontrado: ${Qt6_VERSION}")
endif ()

if (TALLER_TESTS)
    # For Windows: Prevent overriding the parent project's compiler/linker settings
    # (borrowed from https://google.github.io/googletest/quickstart-cmake.html)
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

    # Let's use google test
    include(FetchContent)
    FetchContent_Declare(
            googletest
            URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
            OVERRIDE_FIND_PACKAGE
    )

    FetchContent_MakeAvailable(googletest)
    include(GoogleTest)
endif()
