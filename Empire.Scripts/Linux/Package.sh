#!/usr/bin/env bash
# Использование: ./Package.sh [Режим]  — упаковка билда (заглушка)
source "$(dirname "$0")/Common.sh"
configure "${1:-Shipping}"
xmake build Tools.Packager
echo "[stub] Упаковка: подключить Tools.Packager"
