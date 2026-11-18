#!/usr/bin/env bash
# Использование: ./RunDedicated.command [порт] [Режим]  — сервер без редактора
source "$(dirname "$0")/Common.command"
PORT="${1:-7777}"; MODE="${2:-Shipping}"
EMPIRE_CONFIG_FLAGS="--editor=n ${EMPIRE_CONFIG_FLAGS:-}"
configure "$MODE"
build_tool build Runtime.Server
build_tool run Runtime.Server --port "$PORT"
