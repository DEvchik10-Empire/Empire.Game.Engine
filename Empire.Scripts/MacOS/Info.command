#!/usr/bin/env bash
# Использование: ./Info.command  — версии и окружение
source "$(dirname "$0")/Common.command"
echo "Root: $ROOT"; build_tool --version | head -1; uname -a
for t in cmake clang gcc; do command -v $t >/dev/null && $t --version | head -1; done; true
