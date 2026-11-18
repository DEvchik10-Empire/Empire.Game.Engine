#!/usr/bin/env bash
# Использование: ./CompileAssets.sh  — компиляция ассетов и шейдеров (заглушка)
source "$(dirname "$0")/Common.sh"
xmake build Tools.AssetCompiler Tools.ShaderCompiler
echo "[stub] Запуск компиляторов ассетов/шейдеров"
