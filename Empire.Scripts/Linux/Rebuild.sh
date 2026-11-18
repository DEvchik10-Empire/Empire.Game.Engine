#!/usr/bin/env bash
# Использование: ./Rebuild.sh [Режим] [Таргет]  — полная пересборка
source "$(dirname "$0")/Common.sh"
configure "${1:-}"
xmake clean
if [ -n "${2:-}" ]; then xmake build "$2"; else xmake build; fi
