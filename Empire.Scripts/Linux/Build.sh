#!/usr/bin/env bash
# Использование: ./Build.sh [Режим] [Таргет]
source "$(dirname "$0")/Common.sh"
configure "${1:-}"
if [ -n "${2:-}" ]; then xmake build "$2"; else xmake build; fi
