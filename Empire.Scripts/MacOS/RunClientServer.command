#!/usr/bin/env bash
# Использование: ./RunClientServer.command [Режим]  — сервер в фоне + клиент
source "$(dirname "$0")/Common.command"
MODE="${1:-Development}"
configure "$MODE"
xmake build Runtime.Server Runtime.Client
xmake run Runtime.Server & SPID=$!
trap "kill $SPID 2>/dev/null || true" EXIT
xmake run Runtime.Client
