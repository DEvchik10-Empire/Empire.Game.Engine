#!/usr/bin/env bash
# Использование: ./Setup.sh
set -euo pipefail
# Ставит xmake, если его нет. (заглушка: проверь под свой дистрибутив)
if command -v xmake >/dev/null; then echo "xmake уже есть: $(xmake --version | head -1)"; exit 0; fi
if command -v apt-get >/dev/null; then sudo apt-get install -y xmake || curl -fsSL https://xmake.io/shget.text | bash
elif command -v pacman >/dev/null; then sudo pacman -S --noconfirm xmake
elif command -v dnf >/dev/null; then sudo dnf install -y xmake
else curl -fsSL https://xmake.io/shget.text | bash; fi
