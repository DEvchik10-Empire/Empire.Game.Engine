#!/usr/bin/env bash
# Использование: ./RunEditor.command [Режим] [аргументы программы...]
source "$(dirname "$0")/Common.command"
MODE="${1:-Development}"; [ $# -ge 1 ] && shift
exec "$SCRIPTS/Run.command" Editor "$MODE" "$@"
