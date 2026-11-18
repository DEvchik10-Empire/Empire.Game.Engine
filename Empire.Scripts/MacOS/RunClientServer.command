#!/usr/bin/env bash
# Использование: ./RunClientServer.command [Режим]  — сервер в фоне + клиент; при выходе клиента сервер останавливается
source "$(dirname "$0")/Common.command"
MODE="${1:-Development}"
configure "$MODE"
build_tool build Runtime.Server Runtime.Client
set -m                      # отдельная группа процессов для сервера, чтобы убить и дочерние процессы xmake
build_tool run Runtime.Server & SPID=$!
set +m
cleanup() { kill -- -"$SPID" 2>/dev/null || kill "$SPID" 2>/dev/null || true; }
trap cleanup EXIT INT TERM
build_tool run Runtime.Client
