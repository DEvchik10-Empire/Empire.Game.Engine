#!/usr/bin/env bash
# Использование: ./Configure.sh [Режим]  (EMPIRE_CONFIG_FLAGS для опций)
source "$(dirname "$0")/Common.sh"
configure "${1:-}"
