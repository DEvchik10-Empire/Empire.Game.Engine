#!/usr/bin/env bash
# Использование: ./RunSamples.sh [Режим]  — сборка с --samples=y
source "$(dirname "$0")/Common.sh"
EMPIRE_CONFIG_FLAGS="--samples=y ${EMPIRE_CONFIG_FLAGS:-}"
configure "${1:-}"
xmake build
