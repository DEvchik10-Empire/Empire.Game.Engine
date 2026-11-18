#!/usr/bin/env bash
# Использование: ./RunServer.command [Режим] [аргументы программы...]
source "$(dirname "$0")/Common.command"
MODE="${1:-Development}"; [ $# -ge 1 ] && shift
exec "$SCRIPTS/Run.command" Server "$MODE" "$@"
