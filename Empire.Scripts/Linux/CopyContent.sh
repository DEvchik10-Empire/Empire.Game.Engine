#!/usr/bin/env bash
# Использование: ./CopyContent.sh [Режим]  — скопировать Docs, Content и README в папку сборки
source "$(dirname "$0")/Common.sh"
copy_content "${1:-}"
