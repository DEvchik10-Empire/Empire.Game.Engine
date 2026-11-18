#!/usr/bin/env bash
# Использование: ./RunTests.command  — собрать и прогнать тесты
source "$(dirname "$0")/Common.command"
build_tool f -m test -y ${EMPIRE_CONFIG_FLAGS:-}
build_tool build
build_tool test
