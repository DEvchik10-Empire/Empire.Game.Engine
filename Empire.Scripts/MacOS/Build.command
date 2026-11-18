#!/usr/bin/env bash
# Использование: ./Build.command [Режим] [Таргет]   Режимы: Debug|DebugGame|Development|Test|Shipping
source "$(dirname "$0")/Common.command"
configure "${1:-}"
if [ -n "${2:-}" ]; then build_tool build "$2"; else build_tool build; fi
copy_content "${1:-}"
