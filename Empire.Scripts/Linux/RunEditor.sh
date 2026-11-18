#!/usr/bin/env bash
# Использование: ./RunEditor.sh [Режим] [аргументы программы...]
source "$(dirname "$0")/Common.sh"
MODE="${1:-Development}"; [ $# -ge 1 ] && shift
exec "$SCRIPTS/Run.sh" Editor "$MODE" "$@"
