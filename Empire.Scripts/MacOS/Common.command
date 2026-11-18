#!/usr/bin/env bash
# Общие функции. Подключается через: source "$(dirname "$0")/Common.command"
set -euo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
build_tool() { "${EMPIRE_BUILD_TOOL:-build}" "$@"; }
SCRIPTS="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$ROOT"

# Режим: первый аргумент или Development. Приводится к нижнему регистру.
mode_of() { echo "${1:-Development}" | tr '[:upper:]' '[:lower:]'; }

# build_tool f с режимом и доп. флагами из EMPIRE_CONFIG_FLAGS
configure() { build_tool f -m "$(mode_of "${1:-}")" -y ${EMPIRE_CONFIG_FLAGS:-}; }


# Имя режима с заглавной буквы (как в папке сборки)
mode_name() {
    case "$(mode_of "${1:-}")" in
        debug) echo Debug;; debuggame) echo DebugGame;; development) echo Development;;
        test) echo Test;; shipping) echo Shipping;; *) echo "Неизвестный режим: ${1:-}" >&2; return 1;;
    esac
}

# Копирует Empire.Docs, Empire.Content и README.md в папку сборки (Empire.Build/Build.MacOS.<Режим>/)
copy_content() {
    local root; root="$ROOT/Empire.Build/Build.MacOS.$(mode_name "${1:-}")"
    mkdir -p "$root"
    rm -rf "$root/Build.Docs" "$root/Build.Content"
    cp -R "$ROOT/Empire.Docs" "$root/Build.Docs"
    cp -R "$ROOT/Empire.Content" "$root/Build.Content"
    cp "$ROOT/README.md" "$root/README.md"
    echo "Docs, Content и README скопированы в $root"
}

