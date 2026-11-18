#!/usr/bin/env bash
# Использование: ./BuildShipping.command  — релизная версия (максимальная оптимизация, LTO, без редактора)
exec "$(dirname "$0")/Build.command" Shipping
