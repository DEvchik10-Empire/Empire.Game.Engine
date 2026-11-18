#!/usr/bin/env bash
# Использование: ./RunSamples.command [Режим]  — сборка с --samples=y
source "$(dirname "$0")/Common.command"
EMPIRE_CONFIG_FLAGS="--samples=y ${EMPIRE_CONFIG_FLAGS:-}"
configure "${1:-}"
xmake build
