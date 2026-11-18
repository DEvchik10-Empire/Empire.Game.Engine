#!/usr/bin/env bash
# Использование: ./Rebuild.command [Режим] [Таргет]  — полная пересборка
source "$(dirname "$0")/Common.command"
configure "${1:-}"
build_tool clean
if [ -n "${2:-}" ]; then build_tool build "$2"; else build_tool build; fi
copy_content "${1:-}"
