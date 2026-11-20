#include "Empire.Runtime/Runtime.Tests/Tests.Math/Math.Unit/Unit.Math.hpp"
#include "Empire.Runtime/Runtime.Tests/Tests.Math/Math.Unit/Unit.Check.hpp"

#include "Empire.Engine/Engine.Math/Math.Matrix/Matrix4.hpp"
#include "Empire.Engine/Engine.Math/Math.Quaternion/Quaternion.hpp"

#include <random>

namespace MQ = DEvchik10Empire::EmpireGameEngine::EmpireEngine::EngineMath::MathQuaternion;
namespace MM = DEvchik10Empire::EmpireGameEngine::EmpireEngine::EngineMath::MathMatrix;
namespace MV = DEvchik10Empire::EmpireGameEngine::EmpireEngine::EngineMath::MathVector;
namespace MathScalar = DEvchik10Empire::EmpireGameEngine::EmpireEngine::EngineMath::MathScalar;

void TestQuaternion()
{
    SECTION("Jolt Quaternion");
    using Quaternion = MQ::Quaternion;
    using Vector3 = MV::Vector3;
    using Vector4 = MV::Vector4;
    using Float3 = MV::Float3;
    using Float4 = MV::Float4;
    using Matrix4 = MM::Matrix4;

    static_assert(sizeof(Quaternion) == 16 && alignof(Quaternion) == 16);
    CHECK(Quaternion::sIdentity() == Quaternion(0, 0, 0, 1));
    CHECK(Quaternion::sZero().LengthSq() == 0.0f);

    const Quaternion Rotation = Quaternion::sRotation(Vector3::sAxisZ(), 1.57079632679f);
    CHECK(Rotation.IsNormalized());
    CHECK((Rotation * Vector3::sAxisX()).IsClose(Vector3::sAxisY(), 2.0e-12f));
    CHECK(Rotation.InverseRotate(Vector3::sAxisY()).IsClose(Vector3::sAxisX(), 2.0e-12f));
    CHECK(Rotation.RotateAxisX().IsClose(Vector3::sAxisY(), 2.0e-12f));
    CHECK(Rotation.RotateAxisY().IsClose(-Vector3::sAxisX(), 2.0e-12f));
    CHECK(Rotation.RotateAxisZ().IsClose(Vector3::sAxisZ(), 2.0e-12f));

    Vector3 Axis;
    float Angle = 0.0f;
    Rotation.GetAxisAngle(Axis, Angle);
    CHECK(Axis.IsClose(Vector3::sAxisZ(), 2.0e-12f));
    CHECK_NEAR_TOL(Angle, 1.57079633f, 2.0e-6f);
    CHECK_NEAR_TOL(Rotation.GetRotationAngle(Vector3::sAxisZ()), Angle, 2.0e-6f);
    CHECK(Rotation.GetAngularVelocity(2.0f).IsClose(Vector3(0, 0, 0.7853982f), 2.0e-12f));

    const Quaternion FromTo = Quaternion::sFromTo(Vector3::sAxisX(), Vector3::sAxisY());
    CHECK((FromTo * Vector3::sAxisX()).IsClose(Vector3::sAxisY(), 2.0e-12f));
    CHECK(Quaternion::sFromTo(Vector3::sZero(), Vector3::sAxisY()) == Quaternion::sIdentity());
    const Quaternion HalfTurn = Quaternion::sFromTo(Vector3::sAxisX(), -Vector3::sAxisX());
    CHECK((HalfTurn * Vector3::sAxisX()).IsClose(-Vector3::sAxisX(), 2.0e-12f));

    const Quaternion Euler = Quaternion::sEulerAngles(Vector3(0.2f, -0.3f, 0.4f));
    CHECK(Euler.IsNormalized());
    CHECK(Euler.GetEulerAngles().IsClose(Vector3(0.2f, -0.3f, 0.4f), 2.0e-10f));
    CHECK(Matrix4::sRotation(Euler).GetQuaternion().IsClose(Euler, 2.0e-10f));
    CHECK(Matrix4::sRotation(Euler) == Matrix4::sRotationTranslation(Euler, Vector3::sZero()).GetRotationSafe());
    for (const Vector3& Axis : {Vector3::sAxisX(), Vector3::sAxisY(), Vector3::sAxisZ()})
    {
        const Quaternion HalfTurnRotation = Quaternion::sRotation(Axis, 3.14159265359f);
        CHECK(Matrix4::sRotation(HalfTurnRotation).GetQuaternion().IsClose(HalfTurnRotation, 2.0e-12f));
    }

    const Quaternion Product = Quaternion::sRotation(Vector3::sAxisX(), 0.3f)
        * Quaternion::sRotation(Vector3::sAxisY(), -0.4f);
    CHECK(Product.IsNormalized());
    CHECK_NEAR_TOL(Product.GetPerpendicular().GetXYZW().Dot(Product.GetXYZW()), 0.0f, 1.0e-6f);
    CHECK(Quaternion::sMultiplyImaginary(Vector3::sAxisX(), Quaternion::sIdentity())
        == Quaternion(1, 0, 0, 0));

    const Quaternion Interpolated = Quaternion::sIdentity().SLERP(Rotation, 0.5f);
    CHECK((Interpolated * Vector3::sAxisX()).IsClose(Vector3(0.70710678f, 0.70710678f, 0), 2.0e-12f));
    CHECK(Quaternion::sIdentity().LERP(Rotation, 0.0f) == Quaternion::sIdentity());
    CHECK(Quaternion::sIdentity().SLERP(Rotation, 1.0f).IsClose(Rotation, 2.0e-12f));

    Quaternion Swing;
    Quaternion Twist;
    Rotation.GetSwingTwist(Swing, Twist);
    CHECK((Swing * Twist).IsClose(Rotation, 2.0e-12f));
    CHECK(Rotation.GetTwist(Vector3::sAxisZ()).IsClose(Rotation, 2.0e-12f));
    CHECK(Rotation.EnsureWPositive().GetW() >= 0.0f);
    CHECK(Rotation.Conjugated().IsClose(Quaternion(0, 0, -Rotation.GetZ(), Rotation.GetW()), 2.0e-12f));

    const Float3 Stored3(0.0f, 0.0f, 0.0f);
    const Quaternion Loaded = Quaternion::sLoadFloat3Unsafe(Stored3);
    CHECK(Loaded == Quaternion::sIdentity());
    Float3 Output3;
    (-Rotation).StoreFloat3(&Output3);
    CHECK(Output3 == Float3(0, 0, Rotation.GetZ()));
    Float4 Output4;
    Rotation.StoreFloat4(&Output4);
    CHECK(Output4 == Float4(Rotation.GetX(), Rotation.GetY(), Rotation.GetZ(), Rotation.GetW()));
    const Quaternion Compressed = Quaternion::sDecompressUnitQuat(Rotation.CompressUnitQuat());
    CHECK(Compressed.IsNormalized(1.0e-5f));
    CHECK(Compressed.IsClose(Rotation, 2.0e-5f));

    std::mt19937 ActualRandom(0x51554154u);
    std::mt19937 ExpectedRandom(0x51554154u);
    const auto ExpectedRandomUnit = [&ExpectedRandom]() noexcept
    {
        return static_cast<float>(ExpectedRandom() - ExpectedRandom.min())
            / static_cast<float>(ExpectedRandom.max() - ExpectedRandom.min());
    };
    const float X0 = ExpectedRandomUnit();
    const float R1 = MathScalar::Sqrt(1.0f - X0);
    const float R2 = MathScalar::Sqrt(X0);
    const float Theta1 = MathScalar::TwoPi * ExpectedRandomUnit();
    const float Theta2 = MathScalar::TwoPi * ExpectedRandomUnit();
    Vector4 Sine;
    Vector4 Cosine;
    Vector4(Theta1, Theta2, 0.0f, 0.0f).SinCos(Sine, Cosine);
    CHECK(Quaternion::sRandom(ActualRandom)
        == Quaternion(Sine.GetX() * R1, Cosine.GetX() * R1, Sine.GetY() * R2, Cosine.GetY() * R2));

    std::mt19937 Random(0x51554154u);
    for (int Index = 0; Index < 128; ++Index)
    {
        const Quaternion RandomRotation = Quaternion::sRandom(Random);
        CHECK(RandomRotation.IsNormalized(2.0e-6f));
        CHECK((RandomRotation * RandomRotation.InverseRotate(Vector3(1.2f, -2.3f, 3.4f)))
            .IsClose(Vector3(1.2f, -2.3f, 3.4f), 2.0e-10f));
    }

    CHECK(Matrix4::sQuatLeftMultiply(Rotation) * Vector4(1, 0, 0, 0)
        == Vector4(Rotation.GetW(), Rotation.GetZ(), -Rotation.GetY(), -Rotation.GetX()));
    CHECK(Matrix4::sQuatRightMultiply(Rotation) * Vector4(1, 0, 0, 0)
        == Vector4(Rotation.GetW(), -Rotation.GetZ(), Rotation.GetY(), -Rotation.GetX()));

    const Quaternion Left(0.1f, 0.2f, -0.3f, 0.9f);
    const Quaternion Right(-0.2f, 0.4f, 0.1f, 0.8f);
    CHECK((Matrix4::sQuatLeftMultiply(Left) * Right.GetXYZW()).IsClose((Left * Right).GetXYZW(), 2.0e-12f));
    CHECK((Matrix4::sQuatRightMultiply(Right) * Left.GetXYZW()).IsClose((Left * Right).GetXYZW(), 2.0e-12f));
}
