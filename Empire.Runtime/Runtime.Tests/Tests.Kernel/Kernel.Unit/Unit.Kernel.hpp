#pragma once

// Возвращает 0, если все проверки прошли.
int RunKernelUnitTests();

bool RunCompilerNeonCompileChecks();

bool RunCompilerRISCVCompileChecks();
