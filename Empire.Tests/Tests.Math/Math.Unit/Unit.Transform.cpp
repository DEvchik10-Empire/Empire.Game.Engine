#include "Empire.Tests/Tests.Math/Math.Unit/Unit.Math.hpp"
#include "Empire.Tests/Tests.Math/Math.Unit/Unit.Check.hpp"

#include "Empire.Math/Math.Matrix/Matrix.Matrix4.hpp"
#include "Empire.Math/Math.Quaternion/Quaternion.Quaternion.hpp"
#include "Empire.Math/Math.Scalar/Scalar.hpp"
#include "Empire.Math/Math.Transform/Transform.3D/3D.Transform.hpp"
#include "Empire.Math/Math.Vector/Vector.Vector3.hpp"
#include "Empire.Math/Math.Vector/Vector.Vector4.hpp"

namespace EM = DEvchik10Empire::EmpireGameEngine::EmpireMath;
namespace MS = EM::MathScalar;
using EM::MathMatrix::Matrix4;
using EM::MathQuaternion::Quaternion;
using EM::MathTransform::Transform3D::Transform;
using EM::MathVector::Vector3;
using EM::MathVector::Vector4;

namespace
{
    bool SameMatrix(const Matrix4& A, const Matrix4& B, float T = 2e-3f)
    {
        return MathTests::Near(A.M00, B.M00, T) && MathTests::Near(A.M01, B.M01, T) && MathTests::Near(A.M02, B.M02, T) && MathTests::Near(A.M03, B.M03, T) &&
               MathTests::Near(A.M10, B.M10, T) && MathTests::Near(A.M11, B.M11, T) && MathTests::Near(A.M12, B.M12, T) && MathTests::Near(A.M13, B.M13, T) &&
               MathTests::Near(A.M20, B.M20, T) && MathTests::Near(A.M21, B.M21, T) && MathTests::Near(A.M22, B.M22, T) && MathTests::Near(A.M23, B.M23, T) &&
               MathTests::Near(A.M30, B.M30, T) && MathTests::Near(A.M31, B.M31, T) && MathTests::Near(A.M32, B.M32, T) && MathTests::Near(A.M33, B.M33, T);
    }

    bool SameVector(const Vector3& A, const Vector3& B, float T = 2e-3f)
    {
        return MathTests::Near(A.X, B.X, T) && MathTests::Near(A.Y, B.Y, T) && MathTests::Near(A.Z, B.Z, T);
    }

    Quaternion RandomRotation(MathTests::Random& R)
    {
        const Vector3 Axis(R.Range(-1, 1), R.Range(-1, 1), R.Range(-1, 1));
        return Axis.LengthSquared() < 0.01f ? Quaternion::Identity() : Quaternion::FromAxisAngle(Axis, R.Range(-3.0f, 3.0f));
    }

    Vector3 RandomVector(MathTests::Random& R, float Limit = 8.0f)
    {
        return Vector3(R.Range(-Limit, Limit), R.Range(-Limit, Limit), R.Range(-Limit, Limit));
    }
}

void TestTransform()
{
    SECTION("Transform: основа");
    const Transform Default;
    CHECK(Default.Position == Vector3(0, 0, 0) && Default.Rotation == Quaternion::Identity() && Default.Scale == Vector3(1, 1, 1));
    CHECK(Transform::Identity() == Default);
    const Transform T(Vector3(1, 2, 3), Quaternion::FromAxisAngle(Vector3(0, 1, 0), MS::HalfPi), Vector3(2, 2, 2));
    CHECK(T.Position == Vector3(1, 2, 3) && T.Scale == Vector3(2, 2, 2));
    CHECK(T == Transform(T.Position, T.Rotation, T.Scale));
    CHECK(!(T != Transform(T.Position, T.Rotation, T.Scale)));
    CHECK(T != Default);
    CHECK(Transform(Vector3(1, 2, 3), T.Rotation, T.Scale) == T && Transform(Vector3(1, 2, 4), T.Rotation, T.Scale) != T);

    SECTION("Transform: точки и направления");
    CHECK_VEC3(Default.TransformPoint(Vector3(4, 5, 6)), 4, 5, 6);
    CHECK_VEC3(Transform(Vector3(1, 2, 3), Quaternion::Identity(), Vector3(1, 1, 1)).TransformPoint(Vector3(10, 20, 30)), 11, 22, 33);
    CHECK_VEC3(Transform(Vector3(0, 0, 0), Quaternion::Identity(), Vector3(2, 3, 4)).TransformPoint(Vector3(1, 1, 1)), 2, 3, 4);
    // порядок: масштаб, затем поворот, затем перенос
    CHECK_VEC3(T.TransformPoint(Vector3(1, 0, 0)), 1, 2, 1);
    CHECK_VEC3(T.TransformPoint(Vector3(0, 0, 0)), 1, 2, 3);
    CHECK_VEC3(T.TransformPoint(Vector3(0, 0, 1)), 3, 2, 3);
    // направление не переносится, но поворачивается и масштабируется
    CHECK_VEC3(T.TransformDirection(Vector3(1, 0, 0)), 0, 0, -2);
    CHECK_VEC3(T.TransformDirection(Vector3(0, 0, 0)), 0, 0, 0);
    CHECK_VEC3(Transform(Vector3(9, 9, 9), Quaternion::Identity(), Vector3(1, 1, 1)).TransformDirection(Vector3(1, 2, 3)), 1, 2, 3);

    SECTION("Transform: матрица");
    CHECK(SameMatrix(Default.ToMatrix4(), Matrix4::Identity()));
    CHECK(SameMatrix(T.ToMatrix4(), Matrix4::Translation(T.Position) * T.Rotation.ToMatrix4() * Matrix4::Scale(T.Scale)));

    MathTests::Random Generator(5150);
    for (int Iteration = 0; Iteration < 200; ++Iteration)
    {
        const Transform R(RandomVector(Generator), RandomRotation(Generator), Vector3(Generator.Range(0.3f, 3), Generator.Range(0.3f, 3), Generator.Range(0.3f, 3)));
        const Vector3 P = RandomVector(Generator);
        const Vector4 ByMatrix = R.ToMatrix4() * Vector4(P.X, P.Y, P.Z, 1.0f);
        const Vector3 ByTransform = R.TransformPoint(P);
        CHECK_NEAR_TOL(ByMatrix.X, ByTransform.X, 2e-3f);
        CHECK_NEAR_TOL(ByMatrix.Y, ByTransform.Y, 2e-3f);
        CHECK_NEAR_TOL(ByMatrix.Z, ByTransform.Z, 2e-3f);
        // точка и направление связаны через перенос
        CHECK(SameVector(R.TransformPoint(P) - R.TransformPoint(Vector3(0, 0, 0)), R.TransformDirection(P)));
    }

    SECTION("Transform: обратный");
    CHECK(SameMatrix(Default.Inversed().ToMatrix4(), Matrix4::Identity()));
    CHECK_VEC3(Transform(Vector3(1, 2, 3), Quaternion::Identity(), Vector3(1, 1, 1)).Inversed().Position, -1, -2, -3);
    // обратный точен для равномерного масштаба с любым поворотом
    for (int Iteration = 0; Iteration < 200; ++Iteration)
    {
        const float Uniform = Generator.Range(0.3f, 3.0f);
        const Transform R(RandomVector(Generator), RandomRotation(Generator), Vector3(Uniform, Uniform, Uniform));
        const Transform Inverse = R.Inversed();
        const Vector3 P = RandomVector(Generator);
        CHECK(SameVector(Inverse.TransformPoint(R.TransformPoint(P)), P));
        CHECK(SameVector(R.TransformPoint(Inverse.TransformPoint(P)), P));
        CHECK(SameMatrix(Inverse.ToMatrix4(), R.ToMatrix4().Inversed(), 5e-3f));
        const Transform Round = R * Inverse;
        CHECK(SameVector(Round.TransformPoint(P), P));
    }
    // и для неравномерного масштаба без поворота
    for (int Iteration = 0; Iteration < 100; ++Iteration)
    {
        const Transform R(RandomVector(Generator), Quaternion::Identity(), Vector3(Generator.Range(0.3f, 3), Generator.Range(0.3f, 3), Generator.Range(0.3f, 3)));
        const Vector3 P = RandomVector(Generator);
        CHECK(SameVector(R.Inversed().TransformPoint(R.TransformPoint(P)), P));
    }

    SECTION("Transform: композиция");
    for (int Iteration = 0; Iteration < 200; ++Iteration)
    {
        // родитель с равномерным масштабом, потомок любой
        const float Uniform = Generator.Range(0.3f, 3.0f);
        const Transform Parent(RandomVector(Generator), RandomRotation(Generator), Vector3(Uniform, Uniform, Uniform));
        const Transform Child(RandomVector(Generator), RandomRotation(Generator), Vector3(Generator.Range(0.3f, 3), Generator.Range(0.3f, 3), Generator.Range(0.3f, 3)));
        const Vector3 P = RandomVector(Generator);
        const Transform Combined = Parent * Child;
        CHECK(SameVector(Combined.TransformPoint(P), Parent.TransformPoint(Child.TransformPoint(P))));
        CHECK(SameMatrix(Combined.ToMatrix4(), Parent.ToMatrix4() * Child.ToMatrix4(), 5e-3f));
        Transform Accumulated = Parent;
        Accumulated *= Child;
        CHECK(Accumulated == Combined);

        // родитель с любым масштабом, потомок без поворота
        const Transform Parent2(RandomVector(Generator), RandomRotation(Generator), Vector3(Generator.Range(0.3f, 3), Generator.Range(0.3f, 3), Generator.Range(0.3f, 3)));
        const Transform Child2(RandomVector(Generator), Quaternion::Identity(), Vector3(Generator.Range(0.3f, 3), Generator.Range(0.3f, 3), Generator.Range(0.3f, 3)));
        CHECK(SameVector((Parent2 * Child2).TransformPoint(P), Parent2.TransformPoint(Child2.TransformPoint(P))));
    }
    CHECK((Default * T) == T);
    CHECK((T * Default) == T);
}
