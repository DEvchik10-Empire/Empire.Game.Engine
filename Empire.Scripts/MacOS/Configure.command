#!/usr/bin/env bash
# Использование: ./Configure.command [Режим]  (EMPIRE_CONFIG_FLAGS для опций)
source "$(dirname "$0")/Common.command"
configure "${1:-}"
