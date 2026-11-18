#!/usr/bin/env bash
# Использование: ./RunTests.sh  — собрать и прогнать тесты
source "$(dirname "$0")/Common.sh"
xmake f -m test -y ${EMPIRE_CONFIG_FLAGS:-}
xmake build
xmake test
