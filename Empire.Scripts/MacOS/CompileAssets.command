#!/usr/bin/env bash
# Использование: ./CompileAssets.command  — компиляция ассетов и шейдеров (заглушка)
source "$(dirname "$0")/Common.command"
xmake build Tools.AssetCompiler Tools.ShaderCompiler
echo "[stub] Запуск компиляторов ассетов/шейдеров"
