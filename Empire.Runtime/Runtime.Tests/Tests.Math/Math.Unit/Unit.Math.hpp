#pragma once

// Возвращает 0, если все проверки прошли.
int RunMathUnitTests();

// Тесты по модулям Math (каждая функция в своём Unit.<Модуль>.cpp).
void TestScalar();
void TestColor();
void TestVectorStorage();
void TestVector3();
void TestVector4();
void TestMatrix4();
void TestDynamicMatrix();
void TestSymmetricEigenvalues();
void TestAABB();
void TestConvexHull2D();
void TestConvexHull3D();
void TestQuaternion();
