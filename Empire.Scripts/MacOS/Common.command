#!/usr/bin/env bash
# Общие функции. Подключается через: source "$(dirname "$0")/Common.command"
set -euo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
SCRIPTS="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$ROOT"

# Режим: первый аргумент или Development. Приводится к нижнему регистру.
mode_of() { echo "${1:-Development}" | tr '[:upper:]' '[:lower:]'; }

# xmake f с режимом и доп. флагами из EMPIRE_CONFIG_FLAGS
configure() { xmake f -m "$(mode_of "${1:-}")" -y ${EMPIRE_CONFIG_FLAGS:-}; }

need_xmake() { command -v xmake >/dev/null || { echo "xmake не найден. Запусти Setup.command"; exit 1; }; }
need_xmake
