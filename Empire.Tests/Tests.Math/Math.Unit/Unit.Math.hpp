#pragma once

// Возвращает 0, если все проверки прошли.
int RunMathUnitTests();

// Тесты по модулям Math (каждая функция в своём Unit.<Модуль>.cpp).
void TestScalar();
void TestVector2();
void TestVector3();
void TestVector4();
void TestMatrix2();
void TestMatrix3();
void TestMatrix4();
void TestQuaternion();
void TestTransform();
void TestGeometry();
