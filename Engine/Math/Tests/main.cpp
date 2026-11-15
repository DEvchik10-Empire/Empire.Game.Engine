#include <iostream>
#include <cassert>

#include "Vector3.hpp"
#include "Matrix4.hpp"
#include "Quaternion.hpp"
#include "Transform.hpp"
#include "Math.hpp"

using namespace Math;

void TestVector3()
{
    Vector3 a(1.0f, 2.0f, 3.0f);
    Vector3 b(4.0f, 5.0f, 6.0f);

    // Dot product известное значение: 1*4 + 2*5 + 3*6 = 32
    assert(Math::NearlyEqual(a.Dot(b), 32.0f));

    // Cross product перпендикулярен обоим векторам (dot = 0)
    Vector3 cross = a.Cross(b);
    assert(Math::NearlyEqual(cross.Dot(a), 0.0f, 0.001f));
    assert(Math::NearlyEqual(cross.Dot(b), 0.0f, 0.001f));

    // Normalized вектор имеет длину 1
    Vector3 normalized = a.Normalized();
    assert(Math::NearlyEqual(normalized.Length(), 1.0f, 0.001f));

    std::cout << "Vector3 tests passed\n";
}

void TestMatrix4()
{
    // Identity * Identity == Identity
    Matrix4 identity = Matrix4::Identity();
    Matrix4 result = identity * identity;
    assert(result == identity);

    // Matrix * Inverse(Matrix) == Identity
    Matrix4 translation = Matrix4::Translation({5.0f, 3.0f, 1.0f});
    Matrix4 inverse = translation.Inversed();
    Matrix4 shouldBeIdentity = translation * inverse;

    // Проверяем диагональ ~1, остальное ~0 (с допуском на float погрешность)
    assert(Math::NearlyEqual(shouldBeIdentity.M00, 1.0f, 0.001f));
    assert(Math::NearlyEqual(shouldBeIdentity.M11, 1.0f, 0.001f));
    assert(Math::NearlyEqual(shouldBeIdentity.M22, 1.0f, 0.001f));
    assert(Math::NearlyEqual(shouldBeIdentity.M33, 1.0f, 0.001f));

    // Transposed дважды == оригинал
    Matrix4 rotation = Matrix4::RotationY(1.0f);
    Matrix4 doubleTransposed = rotation.Transposed().Transposed();
    assert(rotation == doubleTransposed);

    std::cout << "Matrix4 tests passed\n";
}

void TestQuaternion()
{
    // Identity quaternion не должен менять вектор при Rotate
    Quaternion identity = Quaternion::Identity();
    Vector3 v(1.0f, 0.0f, 0.0f);
    Vector3 rotated = identity.Rotate(v);
    assert(Math::NearlyEqual(rotated.X, v.X, 0.001f));
    assert(Math::NearlyEqual(rotated.Y, v.Y, 0.001f));
    assert(Math::NearlyEqual(rotated.Z, v.Z, 0.001f));

    // Поворот на 90 градусов вокруг Y должен перевести (1,0,0) в примерно (0,0,-1)
    Quaternion rotation90Y = Quaternion::FromAxisAngle({0.0f, 1.0f, 0.0f}, Math::HalfPi);
    Vector3 rotatedByY = rotation90Y.Rotate({1.0f, 0.0f, 0.0f});
    assert(Math::NearlyEqual(rotatedByY.X, 0.0f, 0.01f));
    assert(Math::NearlyEqual(rotatedByY.Z, -1.0f, 0.01f));

    // Нормализованный кватернион имеет длину 1
    Quaternion q(1.0f, 2.0f, 3.0f, 4.0f);
    Quaternion normalized = q.Normalized();
    assert(Math::NearlyEqual(normalized.Length(), 1.0f, 0.001f));

    std::cout << "Quaternion tests passed\n";
}

void TestPerspective()
{
    // Проверка, что W-компонента после перемножения — не ноль
    // (иначе perspective divide сломан — та самая ошибка, что нашли)
    Matrix4 perspective = Matrix4::Perspective(
        Math::DegreesToRadians(60.0f), 16.0f / 9.0f, 0.1f, 1000.0f
    );

    Vector4 point(0.0f, 0.0f, -5.0f, 1.0f); // точка перед камерой
    Vector4 transformed = perspective * point;

    // W должен быть равен -Z исходной точки (перспективный дивайдер)
    assert(Math::NearlyEqual(transformed.W, 5.0f, 0.01f));

    std::cout << "Perspective test passed\n";
}

int main()
{
    TestVector3();
    TestMatrix4();
    TestQuaternion();
    TestPerspective();

    std::cout << "All math tests passed!\n";
    return 0;
}