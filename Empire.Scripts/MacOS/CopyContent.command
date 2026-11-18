#!/usr/bin/env bash
# Использование: ./CopyContent.command [Режим]  — скопировать Docs, Content и README в папку сборки
source "$(dirname "$0")/Common.command"
copy_content "${1:-}"
