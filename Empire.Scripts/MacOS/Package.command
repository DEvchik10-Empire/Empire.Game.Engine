#!/usr/bin/env bash
# Использование: ./Package.command [Режим]  — упаковка билда (заглушка)
source "$(dirname "$0")/Common.command"
configure "${1:-Shipping}"
xmake build Tools.Packager
echo "[stub] Упаковка: подключить Tools.Packager"
