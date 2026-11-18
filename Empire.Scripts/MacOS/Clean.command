#!/usr/bin/env bash
# Использование: ./Clean.command [All]  — All удаляет ещё и Empire.Build и .xmake
source "$(dirname "$0")/Common.command"
xmake clean || true
if [ "${1:-}" = "All" ]; then rm -rf "$ROOT/Empire.Build" "$ROOT/.xmake" "$ROOT/build"; fi
