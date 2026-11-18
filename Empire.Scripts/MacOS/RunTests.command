#!/usr/bin/env bash
# Использование: ./RunTests.command  — собрать и прогнать тесты
source "$(dirname "$0")/Common.command"
xmake f -m test -y ${EMPIRE_CONFIG_FLAGS:-}
xmake build
xmake test
