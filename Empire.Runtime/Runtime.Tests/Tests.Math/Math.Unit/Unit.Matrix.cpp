#include "Empire.Runtime/Runtime.Tests/Tests.Math/Math.Unit/Unit.Math.hpp"
#include "Empire.Runtime/Runtime.Tests/Tests.Math/Math.Unit/Unit.Check.hpp"

#include "Empire.Engine/Engine.Math/Math.Matrix/Matrix.Matrix2.hpp"
#include "Empire.Engine/Engine.Math/Math.Matrix/Matrix.Matrix3.hpp"
#include "Empire.Engine/Engine.Math/Math.Matrix/Matrix.Matrix4.hpp"
#include "Empire.Engine/Engine.Math/Math.Scalar/Scalar.hpp"
#include "Empire.Engine/Engine.Math/Math.Vector/Vector.Vector2.hpp"
#include "Empire.Engine/Engine.Math/Math.Vector/Vector.Vector3.hpp"
#include "Empire.Engine/Engine.Math/Math.Vector/Vector.Vector4.hpp"

namespace MM = DEvchik10Empire::EmpireGameEngine::EmpireEngine::EngineMath::MathMatrix;
namespace MS = DEvchik10Empire::EmpireGameEngine::EmpireEngine::EngineMath::MathScalar;
using MM::DepthRange;
using MM::Matrix2;
using MM::Matrix3;
using MM::Matrix4;
using DEvchik10Empire::EmpireGameEngine::EmpireEngine::EngineMath::MathVector::Vector2;
using DEvchik10Empire::EmpireGameEngine::EmpireEngine::EngineMath::MathVector::Vector3;
using DEvchik10Empire::EmpireGameEngine::EmpireEngine::EngineMath::MathVector::Vector4;

namespace
{
    constexpr float Tolerance = 1e-3f;

    bool Near(const Matrix2& A, const Matrix2& B, float T = Tolerance)
    {
        return MathTests::Near(A.M00, B.M00, T) && MathTests::Near(A.M01, B.M01, T) &&
               MathTests::Near(A.M10, B.M10, T) && MathTests::Near(A.M11, B.M11, T);
    }

    bool Near(const Matrix3& A, const Matrix3& B, float T = Tolerance)
    {
        return MathTests::Near(A.M00, B.M00, T) && MathTests::Near(A.M01, B.M01, T) && MathTests::Near(A.M02, B.M02, T) &&
               MathTests::Near(A.M10, B.M10, T) && MathTests::Near(A.M11, B.M11, T) && MathTests::Near(A.M12, B.M12, T) &&
               MathTests::Near(A.M20, B.M20, T) && MathTests::Near(A.M21, B.M21, T) && MathTests::Near(A.M22, B.M22, T);
    }

    bool Near(const Matrix4& A, const Matrix4& B, float T = Tolerance)
    {
        return MathTests::Near(A.M00, B.M00, T) && MathTests::Near(A.M01, B.M01, T) && MathTests::Near(A.M02, B.M02, T) && MathTests::Near(A.M03, B.M03, T) &&
               MathTests::Near(A.M10, B.M10, T) && MathTests::Near(A.M11, B.M11, T) && MathTests::Near(A.M12, B.M12, T) && MathTests::Near(A.M13, B.M13, T) &&
               MathTests::Near(A.M20, B.M20, T) && MathTests::Near(A.M21, B.M21, T) && MathTests::Near(A.M22, B.M22, T) && MathTests::Near(A.M23, B.M23, T) &&
               MathTests::Near(A.M30, B.M30, T) && MathTests::Near(A.M31, B.M31, T) && MathTests::Near(A.M32, B.M32, T) && MathTests::Near(A.M33, B.M33, T);
    }

    bool RelativeNear(float A, float B)
    {
        const float Scale = std::fmax(1.0f, std::fmax(std::fabs(A), std::fabs(B)));
        return std::fabs(A - B) <= 1e-3f * Scale;
    }

    Matrix2 RandomMatrix2(MathTests::Random& R)
    {
        return Matrix2(R.Range(-2, 2), R.Range(-2, 2), R.Range(-2, 2), R.Range(-2, 2));
    }

    Matrix3 RandomMatrix3(MathTests::Random& R)
    {
        return Matrix3(R.Range(-2, 2), R.Range(-2, 2), R.Range(-2, 2),
                       R.Range(-2, 2), R.Range(-2, 2), R.Range(-2, 2),
                       R.Range(-2, 2), R.Range(-2, 2), R.Range(-2, 2));
    }

    Matrix4 RandomMatrix4(MathTests::Random& R)
    {
        return Matrix4(R.Range(-2, 2), R.Range(-2, 2), R.Range(-2, 2), R.Range(-2, 2),
                       R.Range(-2, 2), R.Range(-2, 2), R.Range(-2, 2), R.Range(-2, 2),
                       R.Range(-2, 2), R.Range(-2, 2), R.Range(-2, 2), R.Range(-2, 2),
                       R.Range(-2, 2), R.Range(-2, 2), R.Range(-2, 2), R.Range(-2, 2));
    }

    Vector4 Apply(const Matrix4& M, float X, float Y, float Z, float W = 1.0f)
    {
        return M * Vector4(X, Y, Z, W);
    }
}

void TestMatrix2()
{
    SECTION("Matrix2");
    const Matrix2 Identity;
    CHECK(Identity.M00 == 1.0f && Identity.M01 == 0.0f && Identity.M10 == 0.0f && Identity.M11 == 1.0f);
    CHECK(Matrix2::Identity() == Identity);
    const Matrix2 Zero = Matrix2::Zero();
    CHECK(Zero.M00 == 0.0f && Zero.M01 == 0.0f && Zero.M10 == 0.0f && Zero.M11 == 0.0f);

    const Matrix2 A(1.0f, 2.0f, 3.0f, 4.0f);
    CHECK(A.M00 == 1.0f && A.M01 == 2.0f && A.M10 == 3.0f && A.M11 == 4.0f);
    CHECK_NEAR(A.Determinant(), -2.0f);
    CHECK_NEAR(Identity.Determinant(), 1.0f);
    CHECK(Near(A.Transposed(), Matrix2(1.0f, 3.0f, 2.0f, 4.0f)));
    CHECK(A.Transposed().Transposed() == A);

    const Vector2 V = A * Vector2(5.0f, 6.0f);
    CHECK_VEC2(V, 17.0f, 39.0f);
    CHECK(Near(A * Matrix2(5.0f, 6.0f, 7.0f, 8.0f), Matrix2(19.0f, 22.0f, 43.0f, 50.0f)));
    CHECK(A * Identity == A && Identity * A == A);
    CHECK(Near(A + A, A * 2.0f));
    CHECK(Near(A - A, Zero));
    CHECK(Near(A / 2.0f, Matrix2(0.5f, 1.0f, 1.5f, 2.0f)));
    CHECK(Near(2.0f * A, A * 2.0f));

    Matrix2 B = A;
    B += A;
    CHECK(Near(B, Matrix2(2.0f, 4.0f, 6.0f, 8.0f)));
    B -= A;
    CHECK(B == A);
    B *= 3.0f;
    CHECK(Near(B, Matrix2(3.0f, 6.0f, 9.0f, 12.0f)));
    B /= 3.0f;
    CHECK(Near(B, A));
    B *= Matrix2(0.0f, 1.0f, 1.0f, 0.0f);
    CHECK(Near(B, Matrix2(2.0f, 1.0f, 4.0f, 3.0f)));
    CHECK(A == Matrix2(1.0f, 2.0f, 3.0f, 4.0f) && !(A != Matrix2(1.0f, 2.0f, 3.0f, 4.0f)));
    CHECK(A != Matrix2(1.0f, 2.0f, 3.0f, 5.0f));

    // обратная
    CHECK(Near(A.Inversed(), Matrix2(-2.0f, 1.0f, 1.5f, -0.5f)));
    CHECK(Near(A * A.Inversed(), Identity));
    CHECK(Near(Identity.Inversed(), Identity));
    CHECK(Matrix2(1.0f, 2.0f, 2.0f, 4.0f).Inversed() == Zero);
    CHECK(Zero.Inversed() == Zero);

    MathTests::Random Generator(777);
    int Checked = 0;
    for (int Iteration = 0; Iteration < 300; ++Iteration)
    {
        const Matrix2 P = RandomMatrix2(Generator);
        const Matrix2 Q = RandomMatrix2(Generator);
        CHECK(Near(P.Transposed().Transposed(), P));
        CHECK(Near((P * Q).Transposed(), Q.Transposed() * P.Transposed()));
        CHECK(RelativeNear((P * Q).Determinant(), P.Determinant() * Q.Determinant()));
        CHECK(RelativeNear(P.Transposed().Determinant(), P.Determinant()));
        if (std::fabs(P.Determinant()) > 0.3f)
        {
            CHECK(Near(P * P.Inversed(), Identity));
            CHECK(Near(P.Inversed() * P, Identity));
            ++Checked;
        }
    }
    CHECK(Checked > 100);
}

void TestMatrix3()
{
    SECTION("Matrix3");
    const Matrix3 Identity;
    CHECK(Identity.M00 == 1.0f && Identity.M11 == 1.0f && Identity.M22 == 1.0f);
    CHECK(Identity.M01 == 0.0f && Identity.M02 == 0.0f && Identity.M10 == 0.0f && Identity.M12 == 0.0f && Identity.M20 == 0.0f && Identity.M21 == 0.0f);
    CHECK(Matrix3::Identity() == Identity);
    CHECK(Matrix3::Zero() == Matrix3(0, 0, 0, 0, 0, 0, 0, 0, 0));

    const Matrix3 A(2.0f, 0.0f, 1.0f, 1.0f, 3.0f, 2.0f, 1.0f, 1.0f, 4.0f);
    CHECK(A.M00 == 2.0f && A.M01 == 0.0f && A.M02 == 1.0f && A.M10 == 1.0f && A.M11 == 3.0f && A.M12 == 2.0f && A.M20 == 1.0f && A.M21 == 1.0f && A.M22 == 4.0f);
    CHECK_NEAR(A.Determinant(), 18.0f);
    CHECK_NEAR(Identity.Determinant(), 1.0f);
    CHECK_NEAR(Matrix3(1, 2, 3, 4, 5, 6, 7, 8, 9).Determinant(), 0.0f);
    CHECK(Near(A.Transposed(), Matrix3(2, 1, 1, 0, 3, 1, 1, 2, 4)));
    CHECK(A.Transposed().Transposed() == A);

    CHECK_VEC3(A * Vector3(1.0f, 2.0f, 3.0f), 5.0f, 13.0f, 15.0f);
    CHECK(Near(A * Matrix3(1, 2, 3, 4, 5, 6, 7, 8, 9), Matrix3(9, 12, 15, 27, 33, 39, 33, 39, 45)));
    CHECK(A * Identity == A && Identity * A == A);
    CHECK(Near(A + A, A * 2.0f));
    CHECK(Near(A - A, Matrix3::Zero()));
    CHECK(Near((A * 2.0f) / 2.0f, A));
    CHECK(Near(2.0f * A, A * 2.0f));

    Matrix3 B = A;
    B += A;
    CHECK(Near(B, A * 2.0f));
    B -= A;
    CHECK(B == A);
    B *= 2.0f;
    CHECK(Near(B, A * 2.0f));
    B /= 2.0f;
    CHECK(Near(B, A));
    B *= Identity;
    CHECK(B == A);
    CHECK(A != Matrix3::Zero() && !(A != A));

    CHECK(Near(A.Inversed() * A, Identity));
    CHECK(Near(A * A.Inversed(), Identity));
    CHECK(Near(Identity.Inversed(), Identity));
    CHECK(Matrix3(1, 2, 3, 4, 5, 6, 7, 8, 9).Inversed() == Matrix3::Zero());
    CHECK(Matrix3::Zero().Inversed() == Matrix3::Zero());

    MathTests::Random Generator(31337);
    int Checked = 0;
    for (int Iteration = 0; Iteration < 300; ++Iteration)
    {
        const Matrix3 P = RandomMatrix3(Generator);
        const Matrix3 Q = RandomMatrix3(Generator);
        CHECK(Near((P * Q).Transposed(), Q.Transposed() * P.Transposed()));
        CHECK(RelativeNear((P * Q).Determinant(), P.Determinant() * Q.Determinant()));
        CHECK(RelativeNear(P.Transposed().Determinant(), P.Determinant()));
        CHECK(Near((P * Q) * Identity, P * Q));
        if (std::fabs(P.Determinant()) > 0.5f)
        {
            CHECK(Near(P * P.Inversed(), Identity, 5e-3f));
            ++Checked;
        }
    }
    CHECK(Checked > 100);
}

void TestMatrix4()
{
    SECTION("Matrix4: основа");
    const Matrix4 Identity;
    CHECK(Identity.M00 == 1.0f && Identity.M11 == 1.0f && Identity.M22 == 1.0f && Identity.M33 == 1.0f);
    CHECK(Identity.M01 == 0.0f && Identity.M03 == 0.0f && Identity.M10 == 0.0f && Identity.M23 == 0.0f && Identity.M30 == 0.0f && Identity.M32 == 0.0f);
    CHECK(Matrix4::Identity() == Identity);
    const Matrix4 Zero = Matrix4::Zero();
    CHECK(Zero.M00 == 0.0f && Zero.M33 == 0.0f && Zero.M12 == 0.0f);

    const Matrix4 A(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16);
    CHECK(A.M00 == 1 && A.M03 == 4 && A.M10 == 5 && A.M23 == 12 && A.M30 == 13 && A.M33 == 16);
    CHECK(Near(A.Transposed(), Matrix4(1, 5, 9, 13, 2, 6, 10, 14, 3, 7, 11, 15, 4, 8, 12, 16)));
    CHECK(A.Transposed().Transposed() == A);
    CHECK(A * Identity == A && Identity * A == A);
    CHECK(Near(A * A, Matrix4(90, 100, 110, 120, 202, 228, 254, 280, 314, 356, 398, 440, 426, 484, 542, 600)));
    CHECK(Near(A + A, A * 2.0f) && Near(A - A, Zero) && Near((A * 4.0f) / 4.0f, A) && Near(3.0f * A, A * 3.0f));
    Matrix4 B = A;
    B += A;
    CHECK(Near(B, A * 2.0f));
    B -= A;
    CHECK(B == A);
    B *= 2.0f;
    CHECK(Near(B, A * 2.0f));
    B /= 2.0f;
    CHECK(Near(B, A));
    B *= Identity;
    CHECK(B == A);
    CHECK(A != Zero && !(A != A) && !(A == Zero));
    CHECK_VEC4(A * Vector4(1, 0, 0, 0), 1, 5, 9, 13);
    CHECK_VEC4(A * Vector4(1, 1, 1, 1), 10, 26, 42, 58);

    SECTION("Matrix4: определитель и обратная");
    CHECK_NEAR(Identity.Determinant(), 1.0f);
    CHECK_NEAR(Zero.Determinant(), 0.0f);
    CHECK_NEAR(A.Determinant(), 0.0f);
    CHECK_NEAR(Matrix4(2, 0, 0, 0, 0, 3, 0, 0, 0, 0, 4, 0, 0, 0, 0, 5).Determinant(), 120.0f);
    CHECK_NEAR(Matrix4::Translation(Vector3(5, -3, 9)).Determinant(), 1.0f);
    CHECK_NEAR(Matrix4::Scale(Vector3(2, 3, 4)).Determinant(), 24.0f);
    CHECK_NEAR(Matrix4::RotationX(0.7f).Determinant(), 1.0f);
    CHECK_NEAR(Matrix4::RotationY(-1.3f).Determinant(), 1.0f);
    CHECK_NEAR(Matrix4::RotationZ(2.9f).Determinant(), 1.0f);
    CHECK_NEAR(Matrix4(0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1).Determinant(), -1.0f);

    CHECK(Near(Identity.Inversed(), Identity));
    CHECK(Near(Matrix4::Translation(Vector3(5, 3, 1)).Inversed(), Matrix4::Translation(Vector3(-5, -3, -1))));
    CHECK(Near(Matrix4::Scale(Vector3(2, 4, 8)).Inversed(), Matrix4::Scale(Vector3(0.5f, 0.25f, 0.125f))));
    CHECK(Near(Matrix4::RotationY(0.9f).Inversed(), Matrix4::RotationY(0.9f).Transposed()));
    CHECK(Near(Matrix4::RotationX(0.4f).Inversed(), Matrix4::RotationX(-0.4f)));
    CHECK(Zero.Inversed() == Zero);
    CHECK(A.Inversed() == Zero);
    CHECK(Matrix4(1, 2, 3, 4, 1, 2, 3, 4, 0, 0, 1, 0, 0, 0, 0, 1).Inversed() == Zero);
    // ведущий элемент нулевой: нужна перестановка строк
    CHECK(Near(Matrix4(0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1).Inversed(), Matrix4(0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1)));

    MathTests::Random Generator(2024);
    int Checked = 0;
    for (int Iteration = 0; Iteration < 300; ++Iteration)
    {
        const Matrix4 P = RandomMatrix4(Generator);
        const Matrix4 Q = RandomMatrix4(Generator);
        const Matrix4 R = RandomMatrix4(Generator);
        CHECK(Near((P * Q).Transposed(), Q.Transposed() * P.Transposed(), 2e-3f));
        CHECK(Near((P * Q) * R, P * (Q * R), 5e-3f));
        CHECK(RelativeNear((P * Q).Determinant(), P.Determinant() * Q.Determinant()));
        CHECK(RelativeNear(P.Transposed().Determinant(), P.Determinant()));
        CHECK(Near(P * (Q + R), P * Q + P * R, 5e-3f));
        if (std::fabs(P.Determinant()) > 1.0f)
        {
            CHECK(Near(P * P.Inversed(), Identity, 1e-2f));
            CHECK(Near(P.Inversed() * P, Identity, 1e-2f));
            CHECK(RelativeNear(P.Inversed().Determinant(), 1.0f / P.Determinant()));
            ++Checked;
        }
    }
    CHECK(Checked > 100);

    SECTION("Matrix4: перенос, масштаб, повороты");
    CHECK_VEC4(Apply(Matrix4::Translation(Vector3(1, 2, 3)), 10, 20, 30, 1), 11, 22, 33, 1);
    CHECK_VEC4(Apply(Matrix4::Translation(Vector3(1, 2, 3)), 10, 20, 30, 0), 10, 20, 30, 0);
    CHECK_VEC4(Apply(Matrix4::Scale(Vector3(2, 3, 4)), 1, 1, 1, 1), 2, 3, 4, 1);
    CHECK_VEC4(Apply(Matrix4::Scale(Vector3(-1, 1, 1)), 5, 6, 7, 1), -5, 6, 7, 1);

    const float Quarter = MS::HalfPi;
    CHECK_VEC4(Apply(Matrix4::RotationX(Quarter), 0, 1, 0), 0, 0, 1, 1);
    CHECK_VEC4(Apply(Matrix4::RotationX(Quarter), 0, 0, 1), 0, -1, 0, 1);
    CHECK_VEC4(Apply(Matrix4::RotationX(Quarter), 1, 0, 0), 1, 0, 0, 1);
    CHECK_VEC4(Apply(Matrix4::RotationY(Quarter), 0, 0, 1), 1, 0, 0, 1);
    CHECK_VEC4(Apply(Matrix4::RotationY(Quarter), 1, 0, 0), 0, 0, -1, 1);
    CHECK_VEC4(Apply(Matrix4::RotationY(Quarter), 0, 1, 0), 0, 1, 0, 1);
    CHECK_VEC4(Apply(Matrix4::RotationZ(Quarter), 1, 0, 0), 0, 1, 0, 1);
    CHECK_VEC4(Apply(Matrix4::RotationZ(Quarter), 0, 1, 0), -1, 0, 0, 1);
    CHECK_VEC4(Apply(Matrix4::RotationZ(Quarter), 0, 0, 1), 0, 0, 1, 1);
    CHECK(Near(Matrix4::RotationX(0.0f), Identity) && Near(Matrix4::RotationY(0.0f), Identity) && Near(Matrix4::RotationZ(0.0f), Identity));
    CHECK(Near(Matrix4::RotationZ(MS::TwoPi), Identity));

    for (float Angle = -3.0f; Angle <= 3.0f; Angle += 0.5f)
    {
        const Matrix4 RX = Matrix4::RotationX(Angle), RY = Matrix4::RotationY(Angle), RZ = Matrix4::RotationZ(Angle);
        CHECK(Near(RX * RX.Transposed(), Identity));
        CHECK(Near(RY * RY.Transposed(), Identity));
        CHECK(Near(RZ * RZ.Transposed(), Identity));
        CHECK(Near(RY * Matrix4::RotationY(0.3f), Matrix4::RotationY(Angle + 0.3f)));
        // поворот не меняет длину
        const Vector4 Rotated = RX * RY * RZ * Vector4(1, 2, 3, 0);
        CHECK_NEAR_TOL(Vector3(Rotated.X, Rotated.Y, Rotated.Z).Length(), Vector3(1, 2, 3).Length(), 1e-3f);
    }

    const Vector3 Euler(0.3f, -0.8f, 1.1f);
    CHECK(Near(Matrix4::Rotation(Euler), Matrix4::RotationZ(1.1f) * Matrix4::RotationY(-0.8f) * Matrix4::RotationX(0.3f)));
    CHECK(Near(Matrix4::Rotation(Vector3(0, 0, 0)), Identity));
    // порядок: сначала поворот вокруг X, затем Y, затем Z
    CHECK_VEC4(Apply(Matrix4::Rotation(Vector3(Quarter, Quarter, 0)), 0, 1, 0), 1, 0, 0, 1);

    SECTION("Matrix4: проекции");
    {
        const float Fov = MS::HalfPi;
        const Matrix4 P = Matrix4::Perspective(Fov, 2.0f, 1.0f, 100.0f, DepthRange::MinusOneToOne);
        // W = -Z
        CHECK_NEAR(Apply(P, 0, 0, -5).W, 5.0f);
        CHECK_NEAR(Apply(P, 3, 4, -7).W, 7.0f);
        // ближняя плоскость -> -1, дальняя -> +1
        Vector4 Near = Apply(P, 0, 0, -1);
        CHECK_NEAR(Near.Z / Near.W, -1.0f);
        Vector4 Far = Apply(P, 0, 0, -100);
        CHECK_NEAR(Far.Z / Far.W, 1.0f);
        // верх и бок поля зрения
        Vector4 Top = Apply(P, 0, 10, -10);
        CHECK_NEAR(Top.Y / Top.W, 1.0f);
        Vector4 Side = Apply(P, 20, 0, -10);
        CHECK_NEAR(Side.X / Side.W, 1.0f);
        Vector4 Center = Apply(P, 0, 0, -10);
        CHECK_NEAR(Center.X / Center.W, 0.0f);
        CHECK_NEAR(Center.Y / Center.W, 0.0f);
        // глубина растёт монотонно
        float Previous = -2.0f;
        for (float Depth = 1.0f; Depth <= 100.0f; Depth += 3.0f)
        {
            const Vector4 Clip = Apply(P, 0, 0, -Depth);
            const float Ndc = Clip.Z / Clip.W;
            CHECK(Ndc > Previous);
            Previous = Ndc;
        }
        CHECK(::Near(P.Inversed() * P, Identity, 1e-2f));
    }
    {
        const Matrix4 O = Matrix4::Orthographic(-4.0f, 4.0f, -2.0f, 2.0f, 1.0f, 11.0f, DepthRange::MinusOneToOne);
        CHECK_VEC4(Apply(O, -4, -2, -1), -1, -1, -1, 1);
        CHECK_VEC4(Apply(O, 4, 2, -11), 1, 1, 1, 1);
        CHECK_VEC4(Apply(O, 0, 0, -6), 0, 0, 0, 1);
        CHECK_VEC4(Apply(O, 2, 1, -6), 0.5f, 0.5f, 0, 1);
        CHECK(Near(O.Inversed() * O, Identity, 1e-2f));
    }

    SECTION("Matrix4: проекции, глубина 0..1");
    {
        const Matrix4 P = Matrix4::Perspective(MS::HalfPi, 2.0f, 1.0f, 100.0f, DepthRange::ZeroToOne);
        // по умолчанию 0..1
        CHECK(P == Matrix4::Perspective(MS::HalfPi, 2.0f, 1.0f, 100.0f));
        CHECK_NEAR(Apply(P, 0, 0, -5).W, 5.0f);
        const Vector4 Near = Apply(P, 0, 0, -1);
        CHECK_NEAR(Near.Z / Near.W, 0.0f);
        const Vector4 Far = Apply(P, 0, 0, -100);
        CHECK_NEAR_TOL(Far.Z / Far.W, 1.0f, 1e-4f);
        const Vector4 Top = Apply(P, 0, 10, -10);
        CHECK_NEAR(Top.Y / Top.W, 1.0f);
        const Vector4 Side = Apply(P, 20, 0, -10);
        CHECK_NEAR(Side.X / Side.W, 1.0f);
        float Previous = -1.0f;
        for (float Depth = 1.0f; Depth <= 100.0f; Depth += 3.0f)
        {
            const Vector4 Clip = Apply(P, 0, 0, -Depth);
            const float Ndc = Clip.Z / Clip.W;
            CHECK(Ndc > Previous && Ndc >= -1e-5f && Ndc <= 1.0f + 1e-5f);
            Previous = Ndc;
        }
        CHECK(::Near(P.Inversed() * P, Identity, 1e-2f));

        // XY одинаковы в обоих диапазонах, различается только Z
        const Matrix4 G = Matrix4::Perspective(MS::HalfPi, 2.0f, 1.0f, 100.0f, DepthRange::MinusOneToOne);
        CHECK(P.M00 == G.M00 && P.M11 == G.M11 && P.M32 == G.M32 && P.M33 == G.M33);

        const Matrix4 O = Matrix4::Orthographic(-4.0f, 4.0f, -2.0f, 2.0f, 1.0f, 11.0f, DepthRange::ZeroToOne);
        CHECK(O == Matrix4::Orthographic(-4.0f, 4.0f, -2.0f, 2.0f, 1.0f, 11.0f));
        CHECK_VEC4(Apply(O, -4, -2, -1), -1, -1, 0, 1);
        CHECK_VEC4(Apply(O, 4, 2, -11), 1, 1, 1, 1);
        CHECK_VEC4(Apply(O, 0, 0, -6), 0, 0, 0.5f, 1);
        CHECK(::Near(O.Inversed() * O, Identity, 1e-2f));
    }

    SECTION("Matrix4: LookAt");
    {
        const Matrix4 View = Matrix4::LookAt(Vector3(0, 0, 5), Vector3(0, 0, 0), Vector3(0, 1, 0));
        CHECK_VEC4(Apply(View, 0, 0, 5), 0, 0, 0, 1);
        CHECK_VEC4(Apply(View, 0, 0, 0), 0, 0, -5, 1);
        CHECK_VEC4(Apply(View, 1, 0, 5), 1, 0, 0, 1);
        CHECK_VEC4(Apply(View, 0, 1, 5), 0, 1, 0, 1);
        CHECK(Near(Matrix4::LookAt(Vector3(0, 0, 0), Vector3(0, 0, -1), Vector3(0, 1, 0)), Identity));
        CHECK_NEAR(View.Determinant(), 1.0f);

        // взгляд вдоль +X: цель уходит в -Z камеры, мировой +Z становится правым направлением камеры
        const Matrix4 AlongX = Matrix4::LookAt(Vector3(0, 0, 0), Vector3(1, 0, 0), Vector3(0, 1, 0));
        CHECK_VEC4(Apply(AlongX, 5, 0, 0), 0, 0, -5, 1);
        CHECK_VEC4(Apply(AlongX, 0, 0, 1), 1, 0, 0, 1);
        CHECK_VEC4(Apply(AlongX, 0, 1, 0), 0, 1, 0, 1);

        MathTests::Random Generator(99);
        for (int Iteration = 0; Iteration < 200; ++Iteration)
        {
            const Vector3 Eye(Generator.Range(-10, 10), Generator.Range(-10, 10), Generator.Range(-10, 10));
            const Vector3 Target(Generator.Range(-10, 10), Generator.Range(-10, 10), Generator.Range(-10, 10));
            if (Eye.Distance(Target) < 1.0f)
            {
                continue;
            }
            const Matrix4 M = Matrix4::LookAt(Eye, Target, Vector3(0, 1, 0));
            const Vector4 EyeView = Apply(M, Eye.X, Eye.Y, Eye.Z);
            CHECK_VEC4(EyeView, 0, 0, 0, 1);
            const Vector4 TargetView = Apply(M, Target.X, Target.Y, Target.Z);
            CHECK_NEAR_TOL(TargetView.X, 0.0f, 1e-3f);
            CHECK_NEAR_TOL(TargetView.Y, 0.0f, 1e-3f);
            CHECK_NEAR_TOL(TargetView.Z, -Eye.Distance(Target), 1e-3f);
            // вращательная часть ортонормирована
            CHECK_NEAR_TOL(M.Determinant(), 1.0f, 1e-3f);
            CHECK_NEAR_TOL(M.M00 * M.M00 + M.M01 * M.M01 + M.M02 * M.M02, 1.0f, 1e-3f);
            CHECK_NEAR_TOL(M.M00 * M.M10 + M.M01 * M.M11 + M.M02 * M.M12, 0.0f, 1e-3f);
        }
    }
}
