#!/usr/bin/env bash
# Использование: ./BuildShipping.sh  — релизная версия (максимальная оптимизация, LTO, без редактора)
exec "$(dirname "$0")/Build.sh" Shipping
