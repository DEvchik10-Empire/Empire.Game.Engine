#include "Empire.Runtime/Runtime.Tests/Tests.Math/Math.Unit/Unit.Math.hpp"
#include "Empire.Runtime/Runtime.Tests/Tests.Math/Math.Unit/Unit.Check.hpp"

#include "Empire.Engine/Engine.Math/Math.Vector/Vector.Vector2.hpp"
#include "Empire.Engine/Engine.Math/Math.Vector/Vector.Vector3.hpp"
#include "Empire.Engine/Engine.Math/Math.Vector/Vector.Vector4.hpp"

namespace MV = DEvchik10Empire::EmpireGameEngine::EmpireEngine::EngineMath::MathVector;
using MV::Vector2;
using MV::Vector3;
using MV::Vector4;

namespace
{
    // constexpr-конструкторы и операторы должны работать на этапе компиляции
    constexpr Vector2 ConstexprA2(1.0f, 2.0f);
    constexpr Vector2 ConstexprB2 = ConstexprA2 + Vector2(3.0f, 4.0f);
    static_assert(ConstexprB2.X == 4.0f && ConstexprB2.Y == 6.0f);
    constexpr Vector3 ConstexprA3(1.0f, 2.0f, 3.0f);
    constexpr Vector3 ConstexprB3 = ConstexprA3 * 2.0f;
    static_assert(ConstexprB3.X == 2.0f && ConstexprB3.Y == 4.0f && ConstexprB3.Z == 6.0f);
    constexpr Vector4 ConstexprA4(1.0f, 2.0f, 3.0f, 4.0f);
    constexpr Vector4 ConstexprB4 = -ConstexprA4;
    static_assert(ConstexprB4.W == -4.0f);

    Vector3 RandomVector3(MathTests::Random& Generator)
    {
        return Vector3(Generator.Range(-10.0f, 10.0f), Generator.Range(-10.0f, 10.0f), Generator.Range(-10.0f, 10.0f));
    }
}

void TestVector2()
{
    SECTION("Vector2");
    const Vector2 Zero;
    CHECK(Zero.X == 0.0f && Zero.Y == 0.0f);
    const Vector2 A(3.0f, 4.0f);
    CHECK(A.X == 3.0f && A.Y == 4.0f);

    CHECK_NEAR(A.Length(), 5.0f);
    CHECK_NEAR(A.LengthSquared(), 25.0f);
    CHECK_NEAR(Zero.Length(), 0.0f);
    CHECK_VEC2(A.Normalized(), 0.6f, 0.8f);
    CHECK_NEAR(A.Normalized().Length(), 1.0f);
    CHECK_VEC2(Zero.Normalized(), 0.0f, 0.0f);
    CHECK_VEC2(Vector2(-7.0f, 0.0f).Normalized(), -1.0f, 0.0f);

    CHECK_NEAR(Vector2(1.0f, 2.0f).Dot(Vector2(3.0f, 4.0f)), 11.0f);
    CHECK_NEAR(Vector2(1.0f, 0.0f).Dot(Vector2(0.0f, 1.0f)), 0.0f);
    CHECK_NEAR(A.Dot(A), A.LengthSquared());
    CHECK_NEAR(Vector2(1.0f, 2.0f).Dot(Vector2(3.0f, 4.0f)), Vector2(3.0f, 4.0f).Dot(Vector2(1.0f, 2.0f)));

    CHECK_NEAR(Vector2(1.0f, 1.0f).Distance(Vector2(4.0f, 5.0f)), 5.0f);
    CHECK_NEAR(Vector2(1.0f, 1.0f).DistanceSquared(Vector2(4.0f, 5.0f)), 25.0f);
    CHECK_NEAR(A.Distance(A), 0.0f);

    CHECK_VEC2(A + Vector2(1.0f, 2.0f), 4.0f, 6.0f);
    CHECK_VEC2(A - Vector2(1.0f, 2.0f), 2.0f, 2.0f);
    CHECK_VEC2(A * 2.0f, 6.0f, 8.0f);
    CHECK_VEC2(2.0f * A, 6.0f, 8.0f);
    CHECK_VEC2(A / 2.0f, 1.5f, 2.0f);
    CHECK_VEC2(-A, -3.0f, -4.0f);

    Vector2 B = A;
    B += Vector2(1.0f, 1.0f);
    CHECK_VEC2(B, 4.0f, 5.0f);
    B -= Vector2(2.0f, 2.0f);
    CHECK_VEC2(B, 2.0f, 3.0f);
    B *= 3.0f;
    CHECK_VEC2(B, 6.0f, 9.0f);
    B /= 3.0f;
    CHECK_VEC2(B, 2.0f, 3.0f);
    CHECK(&(B += Vector2()) == &B);

    CHECK(A == Vector2(3.0f, 4.0f));
    CHECK(!(A == Vector2(3.0f, 5.0f)));
    CHECK(A != Vector2(4.0f, 4.0f));
    CHECK(!(A != Vector2(3.0f, 4.0f)));
}

void TestVector3()
{
    SECTION("Vector3");
    const Vector3 Zero;
    CHECK(Zero.X == 0.0f && Zero.Y == 0.0f && Zero.Z == 0.0f);
    const Vector3 A(1.0f, 2.0f, 2.0f);
    CHECK(A.X == 1.0f && A.Y == 2.0f && A.Z == 2.0f);

    CHECK_NEAR(A.Length(), 3.0f);
    CHECK_NEAR(A.LengthSquared(), 9.0f);
    CHECK_VEC3(A.Normalized(), 1.0f / 3.0f, 2.0f / 3.0f, 2.0f / 3.0f);
    CHECK_NEAR(A.Normalized().Length(), 1.0f);
    CHECK_VEC3(Zero.Normalized(), 0.0f, 0.0f, 0.0f);
    CHECK_VEC3(Vector3(0.0f, -5.0f, 0.0f).Normalized(), 0.0f, -1.0f, 0.0f);

    CHECK_NEAR(Vector3(1.0f, 2.0f, 3.0f).Dot(Vector3(4.0f, 5.0f, 6.0f)), 32.0f);
    CHECK_NEAR(Vector3(1.0f, 0.0f, 0.0f).Dot(Vector3(0.0f, 1.0f, 0.0f)), 0.0f);
    CHECK_NEAR(Vector3(1.0f, 0.0f, 0.0f).Dot(Vector3(-1.0f, 0.0f, 0.0f)), -1.0f);

    // базис правой системы координат
    const Vector3 X(1.0f, 0.0f, 0.0f), Y(0.0f, 1.0f, 0.0f), Z(0.0f, 0.0f, 1.0f);
    CHECK_VEC3(X.Cross(Y), 0.0f, 0.0f, 1.0f);
    CHECK_VEC3(Y.Cross(Z), 1.0f, 0.0f, 0.0f);
    CHECK_VEC3(Z.Cross(X), 0.0f, 1.0f, 0.0f);
    CHECK_VEC3(Y.Cross(X), 0.0f, 0.0f, -1.0f);
    CHECK_VEC3(Vector3(1.0f, 2.0f, 3.0f).Cross(Vector3(4.0f, 5.0f, 6.0f)), -3.0f, 6.0f, -3.0f);
    CHECK_VEC3(A.Cross(A), 0.0f, 0.0f, 0.0f);
    CHECK_VEC3(A.Cross(A * 3.0f), 0.0f, 0.0f, 0.0f);

    CHECK_NEAR(Vector3(1.0f, 1.0f, 1.0f).Distance(Vector3(4.0f, 5.0f, 1.0f)), 5.0f);
    CHECK_NEAR(Vector3(1.0f, 1.0f, 1.0f).DistanceSquared(Vector3(4.0f, 5.0f, 1.0f)), 25.0f);
    CHECK_NEAR(A.Distance(A), 0.0f);

    CHECK_VEC3(A + Vector3(1.0f, 1.0f, 1.0f), 2.0f, 3.0f, 3.0f);
    CHECK_VEC3(A - Vector3(1.0f, 1.0f, 1.0f), 0.0f, 1.0f, 1.0f);
    CHECK_VEC3(A * 2.0f, 2.0f, 4.0f, 4.0f);
    CHECK_VEC3(2.0f * A, 2.0f, 4.0f, 4.0f);
    CHECK_VEC3(A / 2.0f, 0.5f, 1.0f, 1.0f);
    CHECK_VEC3(-A, -1.0f, -2.0f, -2.0f);

    Vector3 B = A;
    B += Vector3(1.0f, 1.0f, 1.0f);
    CHECK_VEC3(B, 2.0f, 3.0f, 3.0f);
    B -= Vector3(1.0f, 1.0f, 1.0f);
    CHECK_VEC3(B, 1.0f, 2.0f, 2.0f);
    B *= 4.0f;
    CHECK_VEC3(B, 4.0f, 8.0f, 8.0f);
    B /= 4.0f;
    CHECK_VEC3(B, 1.0f, 2.0f, 2.0f);

    CHECK(A == Vector3(1.0f, 2.0f, 2.0f));
    CHECK(!(A == Vector3(1.0f, 2.0f, 3.0f)));
    CHECK(A != Vector3(0.0f, 2.0f, 2.0f));
    CHECK(!(A != Vector3(1.0f, 2.0f, 2.0f)));

    // свойства на случайных векторах
    MathTests::Random Generator(12345);
    for (int Iteration = 0; Iteration < 500; ++Iteration)
    {
        const Vector3 P = RandomVector3(Generator);
        const Vector3 Q = RandomVector3(Generator);
        const Vector3 R = RandomVector3(Generator);

        CHECK_NEAR_TOL(P.Dot(Q), Q.Dot(P), 1e-3f);
        CHECK_NEAR_TOL(P.Dot(Q + R), P.Dot(Q) + P.Dot(R), 1e-2f);
        CHECK_NEAR_TOL(P.Dot(Q * 3.0f), P.Dot(Q) * 3.0f, 1e-2f);

        const Vector3 C = P.Cross(Q);
        CHECK_NEAR_TOL(C.Dot(P), 0.0f, 1e-2f);
        CHECK_NEAR_TOL(C.Dot(Q), 0.0f, 1e-2f);
        const Vector3 Anti = Q.Cross(P);
        CHECK_NEAR_TOL(C.X, -Anti.X, 1e-3f);
        CHECK_NEAR_TOL(C.Y, -Anti.Y, 1e-3f);
        CHECK_NEAR_TOL(C.Z, -Anti.Z, 1e-3f);
        // |P x Q|^2 = |P|^2 |Q|^2 - (P . Q)^2
        const float Lagrange = P.LengthSquared() * Q.LengthSquared() - P.Dot(Q) * P.Dot(Q);
        CHECK_NEAR_TOL(C.LengthSquared(), Lagrange, 0.5f);

        CHECK(P.Length() <= 1e-3f || MathTests::Near(P.Normalized().Length(), 1.0f));
        CHECK((P + Q).Length() <= P.Length() + Q.Length() + 1e-3f);
        CHECK_NEAR_TOL(P.Distance(Q), Q.Distance(P), 1e-4f);
        CHECK_NEAR_TOL(P.Distance(Q), (P - Q).Length(), 1e-4f);
        CHECK_NEAR_TOL((P + Q - Q).X, P.X, 1e-4f);
    }
}

void TestVector4()
{
    SECTION("Vector4");
    const Vector4 Zero;
    CHECK(Zero.X == 0.0f && Zero.Y == 0.0f && Zero.Z == 0.0f && Zero.W == 0.0f);
    const Vector4 A(1.0f, 2.0f, 2.0f, 4.0f);
    CHECK(A.X == 1.0f && A.Y == 2.0f && A.Z == 2.0f && A.W == 4.0f);

    CHECK_NEAR(A.Length(), 5.0f);
    CHECK_NEAR(A.LengthSquared(), 25.0f);
    CHECK_VEC4(A.Normalized(), 0.2f, 0.4f, 0.4f, 0.8f);
    CHECK_NEAR(A.Normalized().Length(), 1.0f);
    CHECK_VEC4(Zero.Normalized(), 0.0f, 0.0f, 0.0f, 0.0f);

    CHECK_NEAR(Vector4(1.0f, 2.0f, 3.0f, 4.0f).Dot(Vector4(5.0f, 6.0f, 7.0f, 8.0f)), 70.0f);
    CHECK_NEAR(Vector4(1.0f, 0.0f, 0.0f, 0.0f).Dot(Vector4(0.0f, 0.0f, 0.0f, 1.0f)), 0.0f);
    CHECK_NEAR(A.Dot(A), A.LengthSquared());

    CHECK_NEAR(Vector4(1.0f, 1.0f, 1.0f, 1.0f).Distance(Vector4(4.0f, 5.0f, 1.0f, 1.0f)), 5.0f);
    CHECK_NEAR(Vector4(1.0f, 1.0f, 1.0f, 1.0f).DistanceSquared(Vector4(4.0f, 5.0f, 1.0f, 1.0f)), 25.0f);

    CHECK_VEC4(A + Vector4(1.0f, 1.0f, 1.0f, 1.0f), 2.0f, 3.0f, 3.0f, 5.0f);
    CHECK_VEC4(A - Vector4(1.0f, 1.0f, 1.0f, 1.0f), 0.0f, 1.0f, 1.0f, 3.0f);
    CHECK_VEC4(A * 2.0f, 2.0f, 4.0f, 4.0f, 8.0f);
    CHECK_VEC4(2.0f * A, 2.0f, 4.0f, 4.0f, 8.0f);
    CHECK_VEC4(A / 2.0f, 0.5f, 1.0f, 1.0f, 2.0f);
    CHECK_VEC4(-A, -1.0f, -2.0f, -2.0f, -4.0f);

    Vector4 B = A;
    B += Vector4(1.0f, 1.0f, 1.0f, 1.0f);
    CHECK_VEC4(B, 2.0f, 3.0f, 3.0f, 5.0f);
    B -= Vector4(1.0f, 1.0f, 1.0f, 1.0f);
    CHECK_VEC4(B, 1.0f, 2.0f, 2.0f, 4.0f);
    B *= 2.0f;
    CHECK_VEC4(B, 2.0f, 4.0f, 4.0f, 8.0f);
    B /= 2.0f;
    CHECK_VEC4(B, 1.0f, 2.0f, 2.0f, 4.0f);

    CHECK(A == Vector4(1.0f, 2.0f, 2.0f, 4.0f));
    CHECK(!(A == Vector4(1.0f, 2.0f, 2.0f, 5.0f)));
    CHECK(A != Vector4(1.0f, 2.0f, 2.0f, 5.0f));
    CHECK(!(A != Vector4(1.0f, 2.0f, 2.0f, 4.0f)));
}
