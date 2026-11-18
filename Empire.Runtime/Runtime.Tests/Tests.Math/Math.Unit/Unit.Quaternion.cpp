#include "Empire.Runtime/Runtime.Tests/Tests.Math/Math.Unit/Unit.Math.hpp"
#include "Empire.Runtime/Runtime.Tests/Tests.Math/Math.Unit/Unit.Check.hpp"

#include "Empire.Engine/Engine.Math/Math.Matrix/Matrix.Matrix4.hpp"
#include "Empire.Engine/Engine.Math/Math.Quaternion/Quaternion.Quaternion.hpp"
#include "Empire.Engine/Engine.Math/Math.Scalar/Scalar.hpp"
#include "Empire.Engine/Engine.Math/Math.Vector/Vector.Vector3.hpp"
#include "Empire.Engine/Engine.Math/Math.Vector/Vector.Vector4.hpp"

namespace EM = DEvchik10Empire::EmpireGameEngine::EmpireEngine::EngineMath;
namespace MS = EM::MathScalar;
using EM::MathMatrix::Matrix4;
using EM::MathQuaternion::Quaternion;
using EM::MathVector::Vector3;
using EM::MathVector::Vector4;

namespace
{
    bool SameRotation(const Quaternion& A, const Quaternion& B)
    {
        // q и -q задают один и тот же поворот
        const Quaternion NA = A.Normalized(), NB = B.Normalized();
        return std::fabs(std::fabs(NA.Dot(NB)) - 1.0f) < 1e-3f;
    }

    bool SameMatrix(const Matrix4& A, const Matrix4& B, float T = 1e-3f)
    {
        return MathTests::Near(A.M00, B.M00, T) && MathTests::Near(A.M01, B.M01, T) && MathTests::Near(A.M02, B.M02, T) && MathTests::Near(A.M03, B.M03, T) &&
               MathTests::Near(A.M10, B.M10, T) && MathTests::Near(A.M11, B.M11, T) && MathTests::Near(A.M12, B.M12, T) && MathTests::Near(A.M13, B.M13, T) &&
               MathTests::Near(A.M20, B.M20, T) && MathTests::Near(A.M21, B.M21, T) && MathTests::Near(A.M22, B.M22, T) && MathTests::Near(A.M23, B.M23, T) &&
               MathTests::Near(A.M30, B.M30, T) && MathTests::Near(A.M31, B.M31, T) && MathTests::Near(A.M32, B.M32, T) && MathTests::Near(A.M33, B.M33, T);
    }

    Quaternion RandomRotation(MathTests::Random& R)
    {
        const Vector3 Axis(R.Range(-1, 1), R.Range(-1, 1), R.Range(-1, 1));
        if (Axis.LengthSquared() < 0.01f)
        {
            return Quaternion::Identity();
        }
        return Quaternion::FromAxisAngle(Axis, R.Range(-3.0f, 3.0f));
    }
}

void TestQuaternion()
{
    SECTION("Quaternion: основа");
    const Quaternion Default;
    CHECK(Default.X == 0.0f && Default.Y == 0.0f && Default.Z == 0.0f && Default.W == 1.0f);
    const Quaternion Identity = Quaternion::Identity();
    CHECK(Identity == Default);
    const Quaternion A(1.0f, 2.0f, 3.0f, 4.0f);
    CHECK(A.X == 1.0f && A.Y == 2.0f && A.Z == 3.0f && A.W == 4.0f);

    CHECK_NEAR(A.LengthSquared(), 30.0f);
    CHECK_NEAR(A.Length(), std::sqrt(30.0f));
    CHECK_NEAR(A.Normalized().Length(), 1.0f);
    CHECK_NEAR(Quaternion(0.0f, 0.0f, 0.0f, 2.0f).Normalized().W, 1.0f);
    CHECK(Quaternion(0.0f, 0.0f, 0.0f, 0.0f).Normalized() == Identity);
    Quaternion N = A;
    N.Normalize();
    CHECK_NEAR(N.Length(), 1.0f);
    CHECK(N == A.Normalized());
    Quaternion ZeroQ(0.0f, 0.0f, 0.0f, 0.0f);
    ZeroQ.Normalize();
    CHECK(ZeroQ == Identity);

    const Quaternion Conjugate = A.Conjugated();
    CHECK(Conjugate.X == -1.0f && Conjugate.Y == -2.0f && Conjugate.Z == -3.0f && Conjugate.W == 4.0f);
    CHECK_NEAR(A.Dot(A), A.LengthSquared());
    CHECK_NEAR(A.Dot(Quaternion(2.0f, 0.0f, 0.0f, 1.0f)), 6.0f);
    CHECK(Quaternion(0.0f, 0.0f, 0.0f, 0.0f).Inversed() == Identity);

    // Inversed: q * q^-1 = единица, в том числе для неединичного q
    for (const Quaternion& Q : {A, Identity, Quaternion(0.5f, -1.0f, 2.0f, 0.1f), Quaternion(0.0f, 0.0f, 3.0f, 0.0f)})
    {
        const Quaternion P = Q * Q.Inversed();
        CHECK(MathTests::Near(P.X, 0.0f) && MathTests::Near(P.Y, 0.0f) && MathTests::Near(P.Z, 0.0f) && MathTests::Near(P.W, 1.0f));
        const Quaternion S = Q.Inversed() * Q;
        CHECK(MathTests::Near(S.X, 0.0f) && MathTests::Near(S.Y, 0.0f) && MathTests::Near(S.Z, 0.0f) && MathTests::Near(S.W, 1.0f));
    }
    // для единичного обратный равен сопряжённому
    const Quaternion Unit = A.Normalized();
    CHECK(SameRotation(Unit.Inversed(), Unit.Conjugated()));
    CHECK_NEAR(Unit.Inversed().X, Unit.Conjugated().X);
    CHECK_NEAR(Unit.Inversed().W, Unit.Conjugated().W);

    SECTION("Quaternion: операторы");
    CHECK(A + Quaternion(1.0f, 1.0f, 1.0f, 1.0f) == Quaternion(2.0f, 3.0f, 4.0f, 5.0f));
    CHECK(A - Quaternion(1.0f, 1.0f, 1.0f, 1.0f) == Quaternion(0.0f, 1.0f, 2.0f, 3.0f));
    CHECK(A * 2.0f == Quaternion(2.0f, 4.0f, 6.0f, 8.0f));
    CHECK(A / 2.0f == Quaternion(0.5f, 1.0f, 1.5f, 2.0f));
    CHECK(-A == Quaternion(-1.0f, -2.0f, -3.0f, -4.0f));
    Quaternion B = A;
    B += A;
    CHECK(B == A * 2.0f);
    B -= A;
    CHECK(B == A);
    B *= 2.0f;
    CHECK(B == A * 2.0f);
    B /= 2.0f;
    CHECK(B == A);
    B *= Identity;
    CHECK(B == A);
    CHECK(A == Quaternion(1.0f, 2.0f, 3.0f, 4.0f) && !(A != Quaternion(1.0f, 2.0f, 3.0f, 4.0f)));
    CHECK(A != Quaternion(1.0f, 2.0f, 3.0f, 5.0f));
    // таблица умножения кватернионных единиц: i*j = k, j*k = i, k*i = j, i*i = -1
    const Quaternion I(1, 0, 0, 0), J(0, 1, 0, 0), K(0, 0, 1, 0);
    CHECK(I * J == K && J * K == I && K * I == J);
    CHECK(J * I == -K);
    CHECK(I * I == Quaternion(0, 0, 0, -1));
    CHECK(A * Identity == A && Identity * A == A);

    SECTION("Quaternion: поворот");
    const float Quarter = MS::HalfPi;
    const Quaternion AroundY = Quaternion::FromAxisAngle(Vector3(0, 1, 0), Quarter);
    CHECK_VEC3(AroundY.Rotate(Vector3(1, 0, 0)), 0, 0, -1);
    CHECK_VEC3(AroundY.Rotate(Vector3(0, 0, 1)), 1, 0, 0);
    CHECK_VEC3(AroundY.Rotate(Vector3(0, 1, 0)), 0, 1, 0);
    const Quaternion AroundX = Quaternion::FromAxisAngle(Vector3(1, 0, 0), Quarter);
    CHECK_VEC3(AroundX.Rotate(Vector3(0, 1, 0)), 0, 0, 1);
    const Quaternion AroundZ = Quaternion::FromAxisAngle(Vector3(0, 0, 1), Quarter);
    CHECK_VEC3(AroundZ.Rotate(Vector3(1, 0, 0)), 0, 1, 0);
    const Quaternion Half = Quaternion::FromAxisAngle(Vector3(0, 0, 1), MS::Pi);
    CHECK_VEC3(Half.Rotate(Vector3(1, 2, 3)), -1, -2, 3);
    // ось можно не нормализовать
    CHECK(SameRotation(Quaternion::FromAxisAngle(Vector3(0, 7, 0), Quarter), AroundY));
    CHECK_NEAR(Quaternion::FromAxisAngle(Vector3(3, 4, 5), 1.0f).Length(), 1.0f);
    // нулевой угол и нулевая ось: ничего не поворачивает
    CHECK(SameRotation(Quaternion::FromAxisAngle(Vector3(1, 2, 3), 0.0f), Identity));
    CHECK_VEC3(Quaternion::FromAxisAngle(Vector3(0, 0, 0), 1.0f).Rotate(Vector3(1, 2, 3)), 1, 2, 3);
    // полный оборот: тот же поворот (кватернион меняет знак)
    CHECK_VEC3(Quaternion::FromAxisAngle(Vector3(0, 1, 0), MS::TwoPi).Rotate(Vector3(1, 2, 3)), 1, 2, 3);
    CHECK_NEAR(Quaternion::FromAxisAngle(Vector3(0, 1, 0), MS::TwoPi).W, -1.0f);
    CHECK_VEC3(Identity.Rotate(Vector3(4, 5, 6)), 4, 5, 6);
    // обратный поворот возвращает вектор
    CHECK_VEC3(AroundY.Inversed().Rotate(AroundY.Rotate(Vector3(1, 2, 3))), 1, 2, 3);
    CHECK_VEC3(AroundY.Conjugated().Rotate(AroundY.Rotate(Vector3(1, 2, 3))), 1, 2, 3);
    // неединичный кватернион поворачивает так же, как нормализованный
    CHECK_VEC3((AroundY * 5.0f).Rotate(Vector3(1, 2, 3)), AroundY.Rotate(Vector3(1, 2, 3)).X, AroundY.Rotate(Vector3(1, 2, 3)).Y, AroundY.Rotate(Vector3(1, 2, 3)).Z);

    MathTests::Random Generator(4242);
    for (int Iteration = 0; Iteration < 300; ++Iteration)
    {
        const Quaternion P = RandomRotation(Generator);
        const Quaternion Q = RandomRotation(Generator);
        const Vector3 V(Generator.Range(-5, 5), Generator.Range(-5, 5), Generator.Range(-5, 5));

        CHECK_NEAR_TOL(P.Length(), 1.0f, 1e-3f);
        CHECK_NEAR_TOL(P.Rotate(V).Length(), V.Length(), 1e-3f);
        // композиция: (P * Q) это сначала Q, потом P
        const Vector3 Combined = (P * Q).Rotate(V);
        const Vector3 Sequential = P.Rotate(Q.Rotate(V));
        CHECK_NEAR_TOL(Combined.X, Sequential.X, 1e-3f);
        CHECK_NEAR_TOL(Combined.Y, Sequential.Y, 1e-3f);
        CHECK_NEAR_TOL(Combined.Z, Sequential.Z, 1e-3f);
        // -q даёт тот же поворот
        const Vector3 Negated = (-P).Rotate(V);
        CHECK_NEAR_TOL(Negated.X, P.Rotate(V).X, 1e-3f);
        // матрица и кватернион поворачивают одинаково
        const Matrix4 M = P.ToMatrix4();
        const Vector4 ByMatrix = M * Vector4(V.X, V.Y, V.Z, 1.0f);
        const Vector3 ByQuaternion = P.Rotate(V);
        CHECK_NEAR_TOL(ByMatrix.X, ByQuaternion.X, 1e-3f);
        CHECK_NEAR_TOL(ByMatrix.Y, ByQuaternion.Y, 1e-3f);
        CHECK_NEAR_TOL(ByMatrix.Z, ByQuaternion.Z, 1e-3f);
        CHECK_NEAR_TOL(ByMatrix.W, 1.0f, 1e-5f);
        // матрица ортонормирована
        CHECK_NEAR_TOL(M.Determinant(), 1.0f, 1e-3f);
        CHECK(SameMatrix(M * M.Transposed(), Matrix4::Identity()));
        // ToMatrix4 нормализует сам
        CHECK(SameMatrix((P * 3.0f).ToMatrix4(), M));
    }

    SECTION("Quaternion: матрица и углы Эйлера");
    CHECK(SameMatrix(Identity.ToMatrix4(), Matrix4::Identity()));
    CHECK(SameMatrix(AroundX.ToMatrix4(), Matrix4::RotationX(Quarter)));
    CHECK(SameMatrix(AroundY.ToMatrix4(), Matrix4::RotationY(Quarter)));
    CHECK(SameMatrix(AroundZ.ToMatrix4(), Matrix4::RotationZ(Quarter)));

    CHECK(SameRotation(Quaternion::FromEulerAngles(Vector3(0, 0, 0)), Identity));
    CHECK(SameRotation(Quaternion::FromEulerAngles(Vector3(Quarter, 0, 0)), AroundX));
    CHECK(SameRotation(Quaternion::FromEulerAngles(Vector3(0, Quarter, 0)), AroundY));
    CHECK(SameRotation(Quaternion::FromEulerAngles(Vector3(0, 0, Quarter)), AroundZ));
    CHECK_NEAR(Quaternion::FromEulerAngles(Vector3(0.4f, -0.9f, 1.7f)).Length(), 1.0f);
    // те же углы дают ту же матрицу, что Matrix4::Rotation (порядок Z * Y * X)
    for (int Iteration = 0; Iteration < 200; ++Iteration)
    {
        const Vector3 Euler(Generator.Range(-3.0f, 3.0f), Generator.Range(-1.4f, 1.4f), Generator.Range(-3.0f, 3.0f));
        const Quaternion Q = Quaternion::FromEulerAngles(Euler);
        CHECK(SameMatrix(Q.ToMatrix4(), Matrix4::Rotation(Euler), 2e-3f));
        // обратное преобразование (Y внутри (-90, 90) градусов)
        const Vector3 Back = Q.ToEulerAngles();
        CHECK_NEAR_TOL(Back.X, Euler.X, 2e-3f);
        CHECK_NEAR_TOL(Back.Y, Euler.Y, 2e-3f);
        CHECK_NEAR_TOL(Back.Z, Euler.Z, 2e-3f);
    }
    CHECK_VEC3(Identity.ToEulerAngles(), 0, 0, 0);
    CHECK_VEC3(AroundX.ToEulerAngles(), Quarter, 0, 0);
    CHECK_VEC3(AroundZ.ToEulerAngles(), 0, 0, Quarter);
    // у границы Y = 90 градусов значение зажимается и не даёт NaN
    const Vector3 Gimbal = Quaternion::FromEulerAngles(Vector3(0, Quarter, 0)).ToEulerAngles();
    CHECK(Gimbal.X == Gimbal.X && Gimbal.Y == Gimbal.Y && Gimbal.Z == Gimbal.Z);
    CHECK_NEAR_TOL(Gimbal.Y, Quarter, 1e-3f);
}
