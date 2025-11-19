if (TALLER_CLIENT OR TALLER_SERVER)
    find_package(cxxopts REQUIRED)
    find_package(spdlog REQUIRED)
endif ()

# Install libSDL2 and libSDL2pp (for client or editor)
if (TALLER_CLIENT OR TALLER_EDITOR OR TALLER_SERVER)
    find_package(SDL2 REQUIRED SDL2_image SDL2_mixer SDL2_ttf)
    find_package(libSDL2pp REQUIRED)
endif ()

if (TALLER_SERVER)
    find_package(box2d REQUIRED)
    find_package(yaml REQUIRED)
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
    find_package(googletest REQUIRED)
    include(GoogleTest)
endif ()
