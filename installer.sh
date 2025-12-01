#!/usr/bin/env bash
BASEDIR="$(dirname "$0")"

# Validar sudo UNA VEZ
sudo -v

# Si corrés con sudo, usar usuario original
if [ "$EUID" -eq 0 ] && [ -n "${SUDO_USER:-}" ]; then
    exec sudo -u "$SUDO_USER" bash "$BASEDIR/installer_internal.sh"
else
    exec bash "$BASEDIR/installer_internal.sh"
fi
