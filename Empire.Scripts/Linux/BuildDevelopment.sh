#!/usr/bin/env bash
# Использование: ./BuildDevelopment.sh  — версия для разработки (с символами, редактор включён)
exec "$(dirname "$0")/Build.sh" Development
