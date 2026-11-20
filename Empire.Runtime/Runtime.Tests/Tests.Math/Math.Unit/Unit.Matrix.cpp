#include "Empire.Runtime/Runtime.Tests/Tests.Math/Math.Unit/Unit.Math.hpp"
#include "Empire.Runtime/Runtime.Tests/Tests.Math/Math.Unit/Unit.Check.hpp"

#include "Empire.Engine/Engine.Math/Math.Matrix/Matrix4.hpp"

namespace MM = DEvchik10Empire::EmpireGameEngine::EmpireEngine::EngineMath::MathMatrix;
namespace MV = DEvchik10Empire::EmpireGameEngine::EmpireEngine::EngineMath::MathVector;
namespace KT = DEvchik10Empire::EmpireGameEngine::EmpireEngine::EngineKernel::KernelTypes;

void TestMatrix4()
{
    SECTION("Jolt Matrix4");
    using Matrix4 = MM::Matrix4;
    using Vector3 = MV::Vector3;
    using Vector4 = MV::Vector4;
    using Float4 = MV::Float4;

    static_assert(sizeof(Matrix4) == 64 && alignof(Matrix4) == 16);
    const Matrix4 Identity = Matrix4::sIdentity();
    CHECK(Identity == Matrix4::sIdentity());
    CHECK(Identity * Vector3(1.0f, 2.0f, 3.0f) == Vector3(1.0f, 2.0f, 3.0f));
    CHECK(Identity * Vector4(1.0f, 2.0f, 3.0f, 1.0f) == Vector4(1.0f, 2.0f, 3.0f, 1.0f));

    const Matrix4 Translation = Matrix4::sTranslation(Vector3(4.0f, -2.0f, 7.0f));
    CHECK(Translation * Vector3(1.0f, 2.0f, 3.0f) == Vector3(5.0f, 0.0f, 10.0f));
    CHECK(Translation.GetTranslation() == Vector3(4.0f, -2.0f, 7.0f));
    CHECK((Translation.InversedRotationTranslation() * (Translation * Vector3(2.0f, 3.0f, 4.0f)))
        .IsClose(Vector3(2.0f, 3.0f, 4.0f)));
    CHECK(Matrix4::sScale(Vector3(2.0f, 3.0f, 4.0f)) * Vector3(1.0f, 2.0f, 3.0f)
        == Vector3(2.0f, 6.0f, 12.0f));

    const Matrix4 Rotation = Matrix4::sRotationZ(1.57079632679f);
    CHECK((Rotation * Vector3::sAxisX()).IsClose(Vector3::sAxisY(), 2.0e-12f));
    CHECK((Rotation * Rotation.InversedRotationTranslation()).IsClose(Identity));
    CHECK((Matrix4::sRotationX(0.3f) * Matrix4::sRotationY(-0.2f)).Transposed().Transposed()
        .IsClose(Matrix4::sRotationX(0.3f) * Matrix4::sRotationY(-0.2f)));
    CHECK((Translation * Translation.Inversed()).IsClose(Identity));

    const Matrix4 Scale = Matrix4::sScale(Vector3(2.0f, 3.0f, 4.0f));
    CHECK_NEAR(Scale.GetDeterminant3x3(), 24.0f);
    CHECK(Scale.Inversed3x3().Multiply3x3(Scale.Multiply3x3(Vector3(1.0f, 2.0f, 3.0f)))
        .IsClose(Vector3(1.0f, 2.0f, 3.0f)));
    Matrix4 Inverse3 = Matrix4::sZero();
    CHECK(Inverse3.SetInversed3x3(Scale));
    CHECK(Inverse3.Multiply3x3(Scale.Multiply3x3(Vector3(3.0f, 2.0f, 1.0f)))
        .IsClose(Vector3(3.0f, 2.0f, 1.0f)));
    CHECK(Inverse3(3, 3) == 1.0f);

    const Matrix4 Perspective = Matrix4::sPerspective(1.0f, 16.0f / 9.0f, 0.1f, 100.0f);
    CHECK(Perspective(3, 2) == -1.0f && Perspective(3, 3) == 0.0f);
    CHECK((Perspective * Perspective.Inversed()).IsClose(Identity, 2.0e-10f));
    CHECK(Matrix4::sOrthographic(-2.0f, 2.0f, -1.0f, 1.0f, 0.1f, 100.0f)
        .IsClose(Matrix4::sOrthographic(-2.0f, 2.0f, -1.0f, 1.0f, 0.1f, 100.0f)));

    Float4 Stored[4];
    Translation.StoreFloat4x4(Stored);
    CHECK(Matrix4::sLoadFloat4x4(Stored) == Translation);
    CHECK(Matrix4::sLoadFloat4x4Aligned(Stored) == Translation);

    const Matrix4 Combined = Translation * Rotation * Scale;
    CHECK((Combined * Combined.Inversed()).IsClose(Identity, 1.0e-10f));
    CHECK((Combined.PostScaled(Vector3(2.0f, 3.0f, 4.0f))
        .IsClose(Matrix4::sScale(Vector3(2.0f, 3.0f, 4.0f)) * Combined)));
    Matrix4 Decomposed = Combined;
    Vector3 ExtractedScale;
    const Matrix4 DecomposedRotation = Decomposed.Decompose(ExtractedScale);
    CHECK(ExtractedScale.IsClose(Vector3(2.0f, 3.0f, 4.0f), 1.0e-10f));
    CHECK(DecomposedRotation.GetTranslation() == Translation.GetTranslation());
    CHECK(DecomposedRotation.GetAxisX().IsNormalized());
    CHECK(DecomposedRotation.GetAxisY().IsNormalized());
    CHECK(DecomposedRotation.GetAxisZ().IsNormalized());

    const Matrix4 Reflected = Matrix4::sScale(Vector3(-2.0f, 3.0f, 4.0f));
    Vector3 ReflectedScale;
    const Matrix4 ReflectedRotation = Reflected.Decompose(ReflectedScale);
    CHECK(ReflectedScale.IsClose(Vector3(2.0f, 3.0f, -4.0f), 1.0e-10f));
    CHECK((ReflectedRotation * Matrix4::sScale(ReflectedScale)).IsClose(Reflected, 1.0e-10f));
    CHECK(Translation.GetRotation() == Identity);
    CHECK(Translation.GetRotationSafe() == Identity);
    const Matrix4 NonAffine(
        Vector4(1, 0, 0, 0.25f), Vector4(0, 1, 0, -0.5f),
        Vector4(0, 0, 1, 0.75f), Vector4(3, 4, 5, 6));
    CHECK(NonAffine.GetRotationSafe() == Identity);

    Matrix4 Singular = Matrix4::sScale(Vector3(0.0f, 1.0f, 1.0f));
    CHECK(!Singular.SetInversed3x3(Singular));
}
