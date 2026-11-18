#include "Empire.Runtime/Runtime.Tests/Tests.Math/Math.Unit/Unit.Math.hpp"
#include "Empire.Runtime/Runtime.Tests/Tests.Math/Math.Unit/Unit.Check.hpp"

#include "Empire.Engine/Engine.Math/Math.Geometry/Geometry.3D/3D.AABB.hpp"
#include "Empire.Engine/Engine.Math/Math.Geometry/Geometry.3D/3D.Bezier.hpp"
#include "Empire.Engine/Engine.Math/Math.Geometry/Geometry.3D/3D.CatmullRom.hpp"
#include "Empire.Engine/Engine.Math/Math.Geometry/Geometry.3D/3D.Frustum.hpp"
#include "Empire.Engine/Engine.Math/Math.Geometry/Geometry.3D/3D.OBB.hpp"
#include "Empire.Engine/Engine.Math/Math.Geometry/Geometry.3D/3D.Plane.hpp"
#include "Empire.Engine/Engine.Math/Math.Geometry/Geometry.3D/3D.Ray.hpp"
#include "Empire.Engine/Engine.Math/Math.Geometry/Geometry.3D/3D.Sphere.hpp"
#include "Empire.Engine/Engine.Math/Math.Matrix/Matrix.Matrix4.hpp"
#include "Empire.Engine/Engine.Math/Math.Quaternion/Quaternion.Quaternion.hpp"
#include "Empire.Engine/Engine.Math/Math.Scalar/Scalar.hpp"
#include "Empire.Engine/Engine.Math/Math.Vector/Vector.Vector3.hpp"
#include "Empire.Engine/Engine.Math/Math.Vector/Vector.Vector4.hpp"

namespace EM = DEvchik10Empire::EmpireGameEngine::EmpireEngine::EngineMath;
namespace MS = EM::MathScalar;
namespace G3 = EM::MathGeometry::Geometry3D;
using EM::MathMatrix::DepthRange;
using EM::MathMatrix::Matrix4;
using EM::MathQuaternion::Quaternion;
using EM::MathVector::Vector3;
using EM::MathVector::Vector4;

namespace
{
    // Точка внутри объёма по clip-space: -w <= x,y,z <= w (глубина -1..1)
    // Margin отсекает точки у самой границы, где float-ошибка меняет ответ
    int ClipVerdict(const Matrix4& VP, const Vector3& P, float Margin, DepthRange Depth)
    {
        const Vector4 C = VP * Vector4(P.X, P.Y, P.Z, 1.0f);
        const float Limit = C.W;
        const float Lower[3] = {-Limit, -Limit, Depth == DepthRange::ZeroToOne ? 0.0f : -Limit};
        const float Values[3] = {C.X, C.Y, C.Z};
        bool Inside = C.W > Margin;
        bool Outside = C.W < -Margin;
        for (int I = 0; I < 3; ++I)
        {
            const float V = Values[I];
            if (V > Limit + Margin || V < Lower[I] - Margin) Outside = true;
            if (V > Limit - Margin || V < Lower[I] + Margin) Inside = false;
        }
        if (C.W < -Margin) return 0;
        if (Outside) return 0;
        return Inside ? 1 : -1;
    }
}

void TestGeometry()
{
    SECTION("Plane");
    const G3::Plane DefaultPlane;
    CHECK(DefaultPlane.Normal == Vector3(0, 1, 0) && DefaultPlane.Distance == 0.0f);
    const G3::Plane Ground(Vector3(0, 1, 0), 2.0f);
    CHECK_NEAR(Ground.DistanceToPoint(Vector3(5, 5, 5)), 3.0f);
    CHECK_NEAR(Ground.DistanceToPoint(Vector3(-4, 2, 9)), 0.0f);
    CHECK_NEAR(Ground.DistanceToPoint(Vector3(0, 0, 0)), -2.0f);
    // конструктор нормализует нормаль
    const G3::Plane Scaled(Vector3(0, 10, 0), 2.0f);
    CHECK_VEC3(Scaled.Normal, 0, 1, 0);
    CHECK(Scaled == Ground && !(Scaled != Ground));
    CHECK(Ground != G3::Plane(Vector3(0, 1, 0), 3.0f));
    CHECK(Ground != G3::Plane(Vector3(1, 0, 0), 2.0f));

    const G3::Plane FromPoint = G3::Plane::FromPointNormal(Vector3(1, 2, 3), Vector3(0, 0, 4));
    CHECK_VEC3(FromPoint.Normal, 0, 0, 1);
    CHECK_NEAR(FromPoint.Distance, 3.0f);
    CHECK_NEAR(FromPoint.DistanceToPoint(Vector3(1, 2, 3)), 0.0f);
    CHECK_NEAR(FromPoint.DistanceToPoint(Vector3(100, -50, 8)), 5.0f);
    MathTests::Random Generator(777);
    for (int Iteration = 0; Iteration < 200; ++Iteration)
    {
        const Vector3 Point(Generator.Range(-9, 9), Generator.Range(-9, 9), Generator.Range(-9, 9));
        Vector3 Normal(Generator.Range(-1, 1), Generator.Range(-1, 1), Generator.Range(-1, 1));
        if (Normal.LengthSquared() < 0.05f) continue;
        const G3::Plane P = G3::Plane::FromPointNormal(Point, Normal);
        CHECK_NEAR_TOL(P.Normal.Length(), 1.0f, 1e-4f);
        CHECK_NEAR_TOL(P.DistanceToPoint(Point), 0.0f, 1e-3f);
        const float Offset = Generator.Range(-5, 5);
        CHECK_NEAR_TOL(P.DistanceToPoint(Point + P.Normal * Offset), Offset, 1e-3f);
    }

    SECTION("Ray");
    const G3::Ray DefaultRay;
    CHECK(DefaultRay.Origin == Vector3(0, 0, 0));
    const G3::Ray R(Vector3(1, 2, 3), Vector3(0, 0, 1));
    CHECK(R.Origin == Vector3(1, 2, 3) && R.Direction == Vector3(0, 0, 1));
    CHECK_VEC3(R.GetPoint(0.0f), 1, 2, 3);
    CHECK_VEC3(R.GetPoint(5.0f), 1, 2, 8);
    CHECK_VEC3(R.GetPoint(-2.0f), 1, 2, 1);
    CHECK(R == G3::Ray(Vector3(1, 2, 3), Vector3(0, 0, 1)) && !(R != G3::Ray(Vector3(1, 2, 3), Vector3(0, 0, 1))));
    CHECK(R != G3::Ray(Vector3(1, 2, 4), Vector3(0, 0, 1)));
    CHECK(R != G3::Ray(Vector3(1, 2, 3), Vector3(0, 1, 0)));

    SECTION("Sphere");
    const G3::Sphere DefaultSphere;
    CHECK(DefaultSphere.Center == Vector3(0, 0, 0) && DefaultSphere.Radius == 1.0f);
    const G3::Sphere S(Vector3(1, 1, 1), 2.0f);
    CHECK(S.Contains(Vector3(1, 1, 1)));
    CHECK(S.Contains(Vector3(3, 1, 1)));
    CHECK(S.Contains(Vector3(2, 2, 2)));
    CHECK(!S.Contains(Vector3(3.01f, 1, 1)));
    CHECK(!S.Contains(Vector3(3, 3, 3)));
    CHECK(S.Intersects(G3::Sphere(Vector3(4, 1, 1), 1.0f)));
    CHECK(S.Intersects(G3::Sphere(Vector3(4.5f, 1, 1), 1.0f)) == false);
    CHECK(S.Intersects(S));
    CHECK(S.Intersects(G3::Sphere(Vector3(1, 1, 1), 0.1f)));
    CHECK(!S.Intersects(G3::Sphere(Vector3(100, 0, 0), 1.0f)));
    CHECK(S == G3::Sphere(Vector3(1, 1, 1), 2.0f) && S != G3::Sphere(Vector3(1, 1, 1), 3.0f) && S != G3::Sphere(Vector3(0, 1, 1), 2.0f));
    for (int Iteration = 0; Iteration < 200; ++Iteration)
    {
        const G3::Sphere A(Vector3(Generator.Range(-5, 5), Generator.Range(-5, 5), Generator.Range(-5, 5)), Generator.Range(0.1f, 3));
        const G3::Sphere B(Vector3(Generator.Range(-5, 5), Generator.Range(-5, 5), Generator.Range(-5, 5)), Generator.Range(0.1f, 3));
        CHECK(A.Intersects(B) == B.Intersects(A));
        const Vector3 P(Generator.Range(-8, 8), Generator.Range(-8, 8), Generator.Range(-8, 8));
        if (A.Contains(P) && B.Contains(P)) CHECK(A.Intersects(B));
    }

    SECTION("AABB");
    const G3::AABB DefaultBox;
    CHECK(DefaultBox.Min == Vector3(0, 0, 0) && DefaultBox.Max == Vector3(0, 0, 0));
    const G3::AABB Box(Vector3(-1, -2, -3), Vector3(3, 2, 1));
    CHECK_VEC3(Box.Center(), 1, 0, -1);
    CHECK_VEC3(Box.Extents(), 2, 2, 2);
    CHECK_VEC3(Box.Size(), 4, 4, 4);
    const G3::AABB FromCenter = G3::AABB::FromCenterExtents(Vector3(1, 0, -1), Vector3(2, 2, 2));
    CHECK(FromCenter == Box && !(FromCenter != Box));
    CHECK(Box != G3::AABB(Vector3(-1, -2, -3), Vector3(3, 2, 2)));
    CHECK(Box.Contains(Vector3(1, 0, -1)));
    CHECK(Box.Contains(Vector3(-1, -2, -3)));
    CHECK(Box.Contains(Vector3(3, 2, 1)));
    CHECK(!Box.Contains(Vector3(3.1f, 0, 0)));
    CHECK(!Box.Contains(Vector3(0, -2.1f, 0)));
    CHECK(!Box.Contains(Vector3(0, 0, 1.1f)));
    CHECK(Box.Intersects(G3::AABB(Vector3(2, 1, 0), Vector3(5, 5, 5))));
    CHECK(Box.Intersects(G3::AABB(Vector3(3, 2, 1), Vector3(5, 5, 5))));
    CHECK(Box.Intersects(G3::AABB(Vector3(0, 0, 0), Vector3(1, 1, 1))));
    CHECK(!Box.Intersects(G3::AABB(Vector3(3.1f, 0, 0), Vector3(5, 5, 5))));
    CHECK(!Box.Intersects(G3::AABB(Vector3(0, 2.1f, 0), Vector3(5, 5, 5))));
    CHECK(!Box.Intersects(G3::AABB(Vector3(0, 0, -9), Vector3(1, 1, -3.1f))));
    const G3::AABB Merged = Box.Union(G3::AABB(Vector3(0, -5, 0), Vector3(10, 0, 2)));
    CHECK(Merged.Min == Vector3(-1, -5, -3) && Merged.Max == Vector3(10, 2, 2));
    CHECK(Box.Union(Box) == Box);
    CHECK(Box.Union(G3::AABB(Vector3(0, 0, 0), Vector3(1, 1, 1))) == Box);
    for (int Iteration = 0; Iteration < 200; ++Iteration)
    {
        const Vector3 C1(Generator.Range(-5, 5), Generator.Range(-5, 5), Generator.Range(-5, 5));
        const Vector3 C2(Generator.Range(-5, 5), Generator.Range(-5, 5), Generator.Range(-5, 5));
        const G3::AABB A = G3::AABB::FromCenterExtents(C1, Vector3(Generator.Range(0.1f, 3), Generator.Range(0.1f, 3), Generator.Range(0.1f, 3)));
        const G3::AABB B = G3::AABB::FromCenterExtents(C2, Vector3(Generator.Range(0.1f, 3), Generator.Range(0.1f, 3), Generator.Range(0.1f, 3)));
        const G3::AABB U = A.Union(B);
        CHECK(A.Intersects(B) == B.Intersects(A));
        CHECK(U == B.Union(A));
        CHECK(U.Contains(A.Min) && U.Contains(A.Max) && U.Contains(B.Min) && U.Contains(B.Max));
        CHECK(A.Intersects(U) && B.Intersects(U));
        const Vector3 P(Generator.Range(-8, 8), Generator.Range(-8, 8), Generator.Range(-8, 8));
        if (A.Contains(P) || B.Contains(P)) CHECK(U.Contains(P));
        if (A.Contains(P) && B.Contains(P)) CHECK(A.Intersects(B));
        CHECK_VEC3(A.Center(), C1.X, C1.Y, C1.Z);
    }

    SECTION("OBB");
    const G3::OBB DefaultObb;
    CHECK(DefaultObb.Center == Vector3(0, 0, 0) && DefaultObb.Extents == Vector3(1, 1, 1) && DefaultObb.Orientation == Quaternion::Identity());
    const G3::OBB Straight(Vector3(1, 2, 3), Vector3(1, 2, 3), Quaternion::Identity());
    CHECK(Straight.Contains(Vector3(1, 2, 3)));
    CHECK(Straight.Contains(Vector3(2, 4, 6)));
    CHECK(!Straight.Contains(Vector3(2.1f, 2, 3)));
    // поворот на 90 градусов вокруг Z меняет X и Y местами
    const G3::OBB Turned(Vector3(0, 0, 0), Vector3(4, 1, 1), Quaternion::FromAxisAngle(Vector3(0, 0, 1), MS::HalfPi));
    CHECK(Turned.Contains(Vector3(0, 3.5f, 0)));
    CHECK(Turned.Contains(Vector3(0, -3.5f, 0)));
    CHECK(!Turned.Contains(Vector3(3.5f, 0, 0)));
    CHECK(!Turned.Contains(Vector3(0, 4.5f, 0)));
    CHECK(Turned.Contains(Vector3(0.9f, 3.9f, 0.9f)));
    CHECK(!Turned.Contains(Vector3(1.1f, 0, 0)));
    // поворот около центра не меняет принадлежность центра
    const G3::OBB Offset(Vector3(10, 0, 0), Vector3(1, 1, 1), Quaternion::FromAxisAngle(Vector3(1, 1, 1), 1.0f));
    CHECK(Offset.Contains(Vector3(10, 0, 0)));
    CHECK(!Offset.Contains(Vector3(0, 0, 0)));
    CHECK(Offset == G3::OBB(Vector3(10, 0, 0), Vector3(1, 1, 1), Quaternion::FromAxisAngle(Vector3(1, 1, 1), 1.0f)));
    CHECK(Offset != Straight);
    // ось-выровненный OBB совпадает с AABB
    for (int Iteration = 0; Iteration < 200; ++Iteration)
    {
        const Vector3 C(Generator.Range(-3, 3), Generator.Range(-3, 3), Generator.Range(-3, 3));
        const Vector3 E(Generator.Range(0.2f, 3), Generator.Range(0.2f, 3), Generator.Range(0.2f, 3));
        const G3::OBB O(C, E, Quaternion::Identity());
        const G3::AABB A = G3::AABB::FromCenterExtents(C, E);
        const Vector3 P(Generator.Range(-8, 8), Generator.Range(-8, 8), Generator.Range(-8, 8));
        CHECK(O.Contains(P) == A.Contains(P));
        // точка из локальных координат внутри бокса принадлежит ему при любом повороте
        const Quaternion Q = Quaternion::FromAxisAngle(Vector3(Generator.Range(-1, 1), 1.0f, Generator.Range(-1, 1)), Generator.Range(-3, 3));
        const G3::OBB Rotated(C, E, Q);
        const Vector3 Local(E.X * 0.9f * Generator.Range(-1, 1), E.Y * 0.9f * Generator.Range(-1, 1), E.Z * 0.9f * Generator.Range(-1, 1));
        CHECK(Rotated.Contains(C + Q.Rotate(Local)));
        const Vector3 Far = Local.Normalized() * (E.Length() * 1.5f);
        if (Local.LengthSquared() > 1e-4f) CHECK(!Rotated.Contains(C + Q.Rotate(Far)));
    }

    SECTION("Bezier");
    const Vector3 P0(0, 0, 0), P1(1, 2, 0), P2(3, 2, 0), P3(4, 0, 0);
    CHECK_VEC3(G3::Bezier::Cubic(P0, P1, P2, P3, 0.0f), 0, 0, 0);
    CHECK_VEC3(G3::Bezier::Cubic(P0, P1, P2, P3, 1.0f), 4, 0, 0);
    CHECK_VEC3(G3::Bezier::Cubic(P0, P1, P2, P3, 0.5f), 2, 1.5f, 0);
    CHECK_VEC3(G3::Bezier::Cubic(P0, P1, P2, P3, 0.25f), 0.90625f, 1.125f, 0);
    // прямая: параметр линейный при равномерно расставленных точках
    const Vector3 L0(0, 0, 0), L1(1, 1, 1), L2(2, 2, 2), L3(3, 3, 3);
    for (int I = 0; I <= 10; ++I)
    {
        const float T = I / 10.0f;
        CHECK_VEC3(G3::Bezier::Cubic(L0, L1, L2, L3, T), 3 * T, 3 * T, 3 * T);
    }
    // одинаковые точки: константа
    CHECK_VEC3(G3::Bezier::Cubic(P1, P1, P1, P1, 0.37f), 1, 2, 0);
    // касательные в концах направлены к соседним контрольным точкам
    const Vector3 Start = G3::Bezier::Cubic(P0, P1, P2, P3, 0.001f);
    CHECK(Start.X > 0 && Start.Y > 0 && Start.Y / Start.X > 1.9f && Start.Y / Start.X < 2.1f);
    for (int Iteration = 0; Iteration < 100; ++Iteration)
    {
        const Vector3 A(Generator.Range(-5, 5), Generator.Range(-5, 5), Generator.Range(-5, 5));
        const Vector3 B(Generator.Range(-5, 5), Generator.Range(-5, 5), Generator.Range(-5, 5));
        const Vector3 C(Generator.Range(-5, 5), Generator.Range(-5, 5), Generator.Range(-5, 5));
        const Vector3 D(Generator.Range(-5, 5), Generator.Range(-5, 5), Generator.Range(-5, 5));
        const float T = Generator.Range(0.0f, 1.0f);
        // симметрия: разворот кривой даёт ту же точку при 1 - T
        const Vector3 Forward = G3::Bezier::Cubic(A, B, C, D, T);
        const Vector3 Reversed = G3::Bezier::Cubic(D, C, B, A, 1.0f - T);
        CHECK_VEC3(Forward, Reversed.X, Reversed.Y, Reversed.Z);
        // выпуклая оболочка контрольных точек
        const float MinX = std::fmin(std::fmin(A.X, B.X), std::fmin(C.X, D.X));
        const float MaxX = std::fmax(std::fmax(A.X, B.X), std::fmax(C.X, D.X));
        CHECK(Forward.X >= MinX - 1e-3f && Forward.X <= MaxX + 1e-3f);
    }

    SECTION("CatmullRom");
    const Vector3 C0(-1, 0, 0), C1(0, 0, 0), C2(1, 1, 0), C3(2, 1, 0);
    CHECK_VEC3(G3::CatmullRom::Interpolate(C0, C1, C2, C3, 0.0f), 0, 0, 0);
    CHECK_VEC3(G3::CatmullRom::Interpolate(C0, C1, C2, C3, 1.0f), 1, 1, 0);
    // середина: (-P0 + 9 P1 + 9 P2 - P3) / 16
    CHECK_VEC3(G3::CatmullRom::Interpolate(C0, C1, C2, C3, 0.5f), 0.5f, 0.5f, 0);
    // равномерные точки на прямой дают линейное движение между P1 и P2
    for (int I = 0; I <= 10; ++I)
    {
        const float T = I / 10.0f;
        CHECK_VEC3(G3::CatmullRom::Interpolate(Vector3(0, 0, 0), Vector3(1, 0, 0), Vector3(2, 0, 0), Vector3(3, 0, 0), T), 1 + T, 0, 0);
    }
    for (int Iteration = 0; Iteration < 100; ++Iteration)
    {
        const Vector3 A(Generator.Range(-5, 5), Generator.Range(-5, 5), Generator.Range(-5, 5));
        const Vector3 B(Generator.Range(-5, 5), Generator.Range(-5, 5), Generator.Range(-5, 5));
        const Vector3 C(Generator.Range(-5, 5), Generator.Range(-5, 5), Generator.Range(-5, 5));
        const Vector3 D(Generator.Range(-5, 5), Generator.Range(-5, 5), Generator.Range(-5, 5));
        CHECK_VEC3(G3::CatmullRom::Interpolate(A, B, C, D, 0.0f), B.X, B.Y, B.Z);
        CHECK_VEC3(G3::CatmullRom::Interpolate(A, B, C, D, 1.0f), C.X, C.Y, C.Z);
        const float T = Generator.Range(0.0f, 1.0f);
        const Vector3 Forward = G3::CatmullRom::Interpolate(A, B, C, D, T);
        const Vector3 Reversed = G3::CatmullRom::Interpolate(D, C, B, A, 1.0f - T);
        CHECK_VEC3(Forward, Reversed.X, Reversed.Y, Reversed.Z);
    }

    SECTION("Frustum");
    // камера в (0,0,5) смотрит в -Z; ближняя 1, дальняя 20
    const Matrix4 View = Matrix4::LookAt(Vector3(0, 0, 5), Vector3(0, 0, 0), Vector3(0, 1, 0));
    const Matrix4 Projection = Matrix4::Perspective(MS::HalfPi, 1.0f, 1.0f, 20.0f, DepthRange::ZeroToOne);
    const Matrix4 VP = Projection * View;
    const G3::Frustum F = G3::Frustum::FromMatrix(VP, DepthRange::ZeroToOne);
    CHECK(F.Near == G3::Frustum::FromMatrix(VP).Near);
    CHECK(F.ContainsPoint(Vector3(0, 0, 0)));
    CHECK(F.ContainsPoint(Vector3(0, 0, -10)));
    CHECK(F.ContainsPoint(Vector3(3, 3, 0)));
    CHECK(!F.ContainsPoint(Vector3(0, 0, 5.5f)));
    CHECK(!F.ContainsPoint(Vector3(0, 0, 4.5f)));
    CHECK(!F.ContainsPoint(Vector3(0, 0, 10)));
    CHECK(!F.ContainsPoint(Vector3(0, 0, -16)));
    CHECK(!F.ContainsPoint(Vector3(0, 0, 4.9f)));
    CHECK(F.ContainsPoint(Vector3(0, 0, 3.9f)));
    CHECK(!F.ContainsPoint(Vector3(20, 0, 0)));
    CHECK(!F.ContainsPoint(Vector3(-20, 0, 0)));
    CHECK(!F.ContainsPoint(Vector3(0, 20, 0)));
    CHECK(!F.ContainsPoint(Vector3(0, -20, 0)));
    // поле зрения 90 градусов: на глубине 5 видно до |x| = 5
    CHECK(F.ContainsPoint(Vector3(4.9f, 0, 0)));
    CHECK(!F.ContainsPoint(Vector3(5.1f, 0, 0)));
    CHECK(F.ContainsPoint(Vector3(0, -4.9f, 0)));
    CHECK(!F.ContainsPoint(Vector3(0, -5.1f, 0)));
    CHECK(F.ContainsPoint(Vector3(0, 4.9f, 0)));
    CHECK(!F.ContainsPoint(Vector3(0, 5.1f, 0)));
    // плоскости нормализованы и смотрят внутрь
    const G3::Plane* Planes[6] = {&F.Near, &F.Far, &F.Left, &F.Right, &F.Top, &F.Bottom};
    for (const G3::Plane* P : Planes)
    {
        CHECK_NEAR_TOL(P->Normal.Length(), 1.0f, 1e-3f);
        CHECK(P->DistanceToPoint(Vector3(0, 0, -5)) > 0.0f);
    }
    CHECK_NEAR_TOL(F.Near.DistanceToPoint(Vector3(0, 0, 3)), 1.0f, 1e-3f);
    CHECK_NEAR_TOL(F.Far.DistanceToPoint(Vector3(0, 0, -10)), 5.0f, 1e-3f);
    CHECK_NEAR_TOL(F.Near.DistanceToPoint(Vector3(7, -3, 4)), 0.0f, 1e-3f);

    // случайные точки против проверки в clip-space
    const Vector3 Eyes[3] = {Vector3(0, 0, 5), Vector3(3, 2, -4), Vector3(-6, 4, 1)};
    for (const DepthRange Range : {DepthRange::MinusOneToOne, DepthRange::ZeroToOne})
    for (const Vector3& Eye : Eyes)
    {
        const Matrix4 V = Matrix4::LookAt(Eye, Vector3(0, 0, 0), Vector3(0, 1, 0));
        const Matrix4 P = Matrix4::Perspective(1.1f, 1.6f, 0.5f, 30.0f, Range);
        const Matrix4 M = P * V;
        const G3::Frustum Fr = G3::Frustum::FromMatrix(M, Range);
        for (int Iteration = 0; Iteration < 1500; ++Iteration)
        {
            const Vector3 Point(Generator.Range(-25, 25), Generator.Range(-25, 25), Generator.Range(-25, 25));
            const int Verdict = ClipVerdict(M, Point, 0.05f, Range);
            if (Verdict == -1) continue;
            CHECK(Fr.ContainsPoint(Point) == (Verdict == 1));
        }
    }

    // ортографическая матрица: коробка
    const Matrix4 Ortho = Matrix4::Orthographic(-2, 2, -1, 1, 0.5f, 10.0f, DepthRange::MinusOneToOne);
    const G3::Frustum Box2 = G3::Frustum::FromMatrix(Ortho, DepthRange::MinusOneToOne);
    CHECK(Box2.ContainsPoint(Vector3(0, 0, -5)));
    CHECK(Box2.ContainsPoint(Vector3(1.9f, 0.9f, -9.5f)));
    CHECK(!Box2.ContainsPoint(Vector3(2.1f, 0, -5)));
    CHECK(!Box2.ContainsPoint(Vector3(0, 1.1f, -5)));
    CHECK(!Box2.ContainsPoint(Vector3(0, 0, -0.4f)));
    CHECK(!Box2.ContainsPoint(Vector3(0, 0, -10.5f)));
    CHECK(!Box2.ContainsPoint(Vector3(0, 0, 1)));
    const G3::Frustum Box3 = G3::Frustum::FromMatrix(Matrix4::Orthographic(-2, 2, -1, 1, 0.5f, 10.0f, DepthRange::ZeroToOne), DepthRange::ZeroToOne);
    for (float Z : {-0.6f, -5.0f, -9.9f}) CHECK(Box3.ContainsPoint(Vector3(0, 0, Z)));
    for (float Z : {-0.4f, -10.1f, 1.0f}) CHECK(!Box3.ContainsPoint(Vector3(0, 0, Z)));
    CHECK(!Box3.ContainsPoint(Vector3(2.1f, 0, -5)) && !Box3.ContainsPoint(Vector3(0, 1.1f, -5)));
}
