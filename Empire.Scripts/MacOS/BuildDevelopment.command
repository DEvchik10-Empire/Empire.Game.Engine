#!/usr/bin/env bash
# Использование: ./BuildDevelopment.command  — версия для разработки (с символами, редактор включён)
exec "$(dirname "$0")/Build.command" Development
