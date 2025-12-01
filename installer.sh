#!/usr/bin/env bash

# Instalador simple del proyecto "need-for-speed-g25"
#
# Dar permisos de ejecución:
#   chmod +x installer.sh
# Ejecutar:
#   ./installer.sh


# para que el script falle si hay errores
set -euo pipefail

BLUE='\033[0;34m'
NC='\033[0m'

PROJECT_NAME="need-for-speed-g25"

have_cmd() { command -v "$1" >/dev/null 2>&1; }

say() { printf "${BLUE}[$PROJECT_NAME]${NC} $* "; }

spinner() {
    local pid=$1        # PID del proceso a esperar
    local delay=0.1     # velocidad del spinner
    local spinstr='|/-\'
    printf "[ ]"
    while kill -0 "$pid" 2>/dev/null; do
        for i in $(seq 0 3); do
            printf "\b\b\b[${spinstr:i:1}]"
            sleep $delay
        done
    done
    printf "\b\b\b[✔]\n"
}

_apt() {
  DEBIAN_FRONTEND=noninteractive apt-get -y -qq --no-install-recommends "$@"
}

install_base() {
  _apt install apt-utils > /dev/null 2>&1
  _apt install \
    build-essential git unzip pkg-config ca-certificates \
    ninja-build python3 python3-pip \
    gcc g++ rsync wget gpg software-properties-common wget gnupg2
}
install_cmake() {
  # jammy (compatible Mint + Ubuntu 22/24)
  wget -qO- https://apt.kitware.com/keys/kitware-archive-latest.asc \
      | sudo gpg --dearmor -o /usr/share/keyrings/kitware.gpg

  echo "deb [signed-by=/usr/share/keyrings/kitware.gpg] https://apt.kitware.com/ubuntu/ jammy main" \
      | sudo tee /etc/apt/sources.list.d/kitware.list

  _apt update
  _apt install cmake
}
install_compiler() {
  add-apt-repository -y ppa:ubuntu-toolchain-r/test
  _apt update
  _apt install g++-13
  update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-13 100
}
install_multimedia_dependencies() {
  _apt install \
    libasound2t64 libasound2-dev pulseaudio \
    libopusfile0 libopusfile-dev libmodplug1 libmodplug-dev \
    libsndfile1 libsndfile1-dev libmpg123-0 libmpg123-dev \
    libogg0 libogg-dev libvorbis0a libvorbisenc2 libvorbis-dev \
    libopus0 libopus-dev libwavpack-dev libxmp-dev libfluidsynth-dev libfreetype6-dev

  _apt install \
    libx11-6 libx11-dev libxext6 libxext-dev \
    libgl1-mesa-dri libgl1-mesa-dev libglu1-mesa-dev

  _apt install gnome-terminal
}
install_qt() {
  sudo add-apt-repository ppa:okirby/qt6-backports
  _apt install \
    qt6-base-dev qt6-tools-dev qt6-tools-dev-tools
}
prepare_system() {
  say "Actualizando indices"
  _apt update > /dev/null &
  spinner $!

  say "Instalando herramientas base"
  install_base  > /dev/null &
  spinner $!

  say "Instalando g++-13"
  install_compiler > /dev/null &
  spinner $!

  say "Instalando cmake actualizado"
  install_cmake > /dev/null &
  spinner $!

  say "Instalando librerías multimedia y audio"
  install_multimedia_dependencies > /dev/null &
  spinner $!

  # Qt6
  say "Instalando Qt6 (Core/Widgets y herramientas)"
  install_qt > /dev/null &
  spinner $!
}

configure_build() {
  say "Configurando el proyecto (Release)"
  local generator="Unix Makefiles"
  if have_cmd ninja; then generator="Ninja"; fi # ninja es más rápido

  cmake -S . -B build -G "${generator}" \
    -DCMAKE_BUILD_TYPE=Release \
    -DTALLER_CLIENT=ON -DTALLER_SERVER=ON -DTALLER_EDITOR=ON -DTALLER_TESTS=ON \ -Wno-dev -Wno-deprecated \ > /dev/null &
  spinner $!
}

build_all() {
  say "Compilando"
  make build > /dev/null &
  spinner $!
}

run_tests() {
  say "Ejecutando tests"
  make tests &
}

print_summary() {
  echo "Puedes lanzar el servidor con 'make exec-server'"
  echo "Puedes lanzar un cliente con 'make exec-client'"
}

main() {
  prepare_system
  configure_build
  build_all
  run_tests
  print_summary
}

main "$@"
