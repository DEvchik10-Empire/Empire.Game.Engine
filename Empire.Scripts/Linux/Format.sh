#!/usr/bin/env bash
# Использование: ./Format.sh  — clang-format по .hpp/.cpp (заглушка)
source "$(dirname "$0")/Common.sh"
command -v clang-format >/dev/null || { echo "нет clang-format"; exit 1; }
find Empire.* \( -name "*.hpp" -o -name "*.cpp" \) -not -path "*ThirdParty*" -print0 | xargs -0 clang-format -i
