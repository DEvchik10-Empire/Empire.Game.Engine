#!/usr/bin/env bash
# Использование: ./Build.command [Режим] [Таргет]
source "$(dirname "$0")/Common.command"
configure "${1:-}"
if [ -n "${2:-}" ]; then xmake build "$2"; else xmake build; fi
