#!/usr/bin/env bash
# Использование: ./Run.command [Game|Client|Server|Editor|Tests] [Режим] [аргументы...]
source "$(dirname "$0")/Common.command"
TARGET="${1:-Game}"; MODE="${2:-Development}"
if [ $# -ge 2 ]; then shift 2; else shift $#; fi
if [ "$TARGET" = "Tests" ]; then
    build_tool f -m test -y ${EMPIRE_CONFIG_FLAGS:-}; build_tool build; build_tool test; exit 0
fi
configure "$MODE"
build_tool build "Runtime.$TARGET"
build_tool run "Runtime.$TARGET" "$@"
