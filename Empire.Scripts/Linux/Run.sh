#!/usr/bin/env bash
# Использование: ./Run.sh [Game|Client|Server|Editor|Tests] [Режим] [аргументы...]
source "$(dirname "$0")/Common.sh"
TARGET="${1:-Game}"; MODE="${2:-Development}"
if [ $# -ge 2 ]; then shift 2; else shift $#; fi
if [ "$TARGET" = "Tests" ]; then
    xmake f -m test -y ${EMPIRE_CONFIG_FLAGS:-}; xmake build; xmake test; exit 0
fi
configure "$MODE"
xmake build "Runtime.$TARGET"
xmake run "Runtime.$TARGET" "$@"
