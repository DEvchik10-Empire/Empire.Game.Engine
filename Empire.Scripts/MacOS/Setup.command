#!/usr/bin/env bash
# Использование: ./Setup.command
set -euo pipefail
# Ставит xmake, если его нет. (заглушка: проверь под свой дистрибутив)
if command -v xmake >/dev/null; then echo "xmake уже есть: $(xmake --version | head -1)"; exit 0; fi
if command -v brew >/dev/null; then brew install xmake; else curl -fsSL https://xmake.io/shget.text | bash; fi
