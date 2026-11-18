#!/usr/bin/env bash
# Использование: ./RunTests.sh  — собрать и прогнать тесты
source "$(dirname "$0")/Common.sh"
build_tool f -m test -y ${EMPIRE_CONFIG_FLAGS:-}
build_tool build
build_tool test
