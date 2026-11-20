#include "Empire.Runtime/Runtime.Tests/Tests.Math/Math.Unit/Unit.Math.hpp"
#include "Empire.Runtime/Runtime.Tests/Tests.Math/Math.Unit/Unit.Check.hpp"

#include "Empire.Engine/Engine.Kernel/Kernel.Containers/Containers.Array.hpp"
#include "Empire.Engine/Engine.Math/Math.Geometry/Geometry.3D/AABB.hpp"
#include "Empire.Engine/Engine.Math/Math.Geometry/Geometry.3D/Plane.hpp"
#include "Empire.Engine/Engine.Math/Math.Geometry/Geometry.3D/Sphere.hpp"
#include "Empire.Engine/Engine.Math/Math.Geometry/Geometry.3D/OBB.hpp"
#include "Empire.Engine/Engine.Math/Math.Geometry/Geometry.3D/Ellipse.hpp"
#include "Empire.Engine/Engine.Math/Math.Geometry/Geometry.3D/MortonCode.hpp"
#include "Empire.Engine/Engine.Math/Math.Geometry/Geometry.3D/RayAABox.hpp"
#include "Empire.Engine/Engine.Math/Math.Geometry/Geometry.3D/RaySphere.hpp"
#include "Empire.Engine/Engine.Math/Math.Geometry/Geometry.3D/RayTriangle.hpp"
#include "Empire.Engine/Engine.Math/Math.Geometry/Geometry.3D/RayCylinder.hpp"
#include "Empire.Engine/Engine.Math/Math.Geometry/Geometry.3D/RayCapsule.hpp"
#include "Empire.Engine/Engine.Math/Math.Geometry/Geometry.3D/ClosestPoint.hpp"
#include "Empire.Engine/Engine.Math/Math.Geometry/Geometry.3D/ClipPoly.hpp"
#include "Empire.Engine/Engine.Math/Math.Geometry/Geometry.3D/AABB4.hpp"
#include "Empire.Engine/Engine.Math/Math.Geometry/Geometry.3D/Indexify.hpp"

namespace MG = DEvchik10Empire::EmpireGameEngine::EmpireEngine::EngineMath::Geometry3D;
namespace MM = DEvchik10Empire::EmpireGameEngine::EmpireEngine::EngineMath::MathMatrix;
namespace MV = DEvchik10Empire::EmpireGameEngine::EmpireEngine::EngineMath::MathVector;
namespace KC = DEvchik10Empire::EmpireGameEngine::EmpireEngine::EngineKernel::KernelContainers;
namespace KT = DEvchik10Empire::EmpireGameEngine::EmpireEngine::EngineKernel::KernelTypes;

void TestAABB()
{
    SECTION("Jolt axis-aligned bounding boxes");
    using AABB = MG::AABB;
    using Vector3 = MV::Vector3;
    using Vector4 = MV::Vector4;
    using UVector4 = MV::UVector4;
    using Matrix4 = MM::Matrix4;

    AABB Empty;
    CHECK(!Empty.IsValid());
    CHECK(Empty == AABB(Vector3::sReplicate(3.402823466e38f), Vector3::sReplicate(-3.402823466e38f)));

    AABB Box = AABB::sFromTwoPoints(Vector3(-1, -2, -3), Vector3(3, 4, 5));
    CHECK(Box.IsValid());
    CHECK(Box.GetCenter() == Vector3(1, 1, 1));
    CHECK(Box.GetExtent() == Vector3(2, 3, 4));
    CHECK(Box.GetSize() == Vector3(4, 6, 8));
    CHECK(Box.GetSurfaceArea() == 208.0f);
    CHECK(Box.GetVolume() == 192.0f);
    CHECK(Box.Contains(Vector3::sZero()));
    CHECK(Box.Contains(AABB(Vector3(-1, -1, -1), Vector3(1, 1, 1))));
    CHECK(Box.Overlaps(AABB(Vector3(3, 4, 5), Vector3(6, 7, 8))));
    CHECK(!Box.Overlaps(AABB(Vector3(3.1f, 4, 5), Vector3(6, 7, 8))));
    CHECK(Box.GetClosestPoint(Vector3(5, 0, -8)) == Vector3(3, 0, -3));
    CHECK(Box.GetSqDistanceTo(Vector3(5, 0, -8)) == 29.0f);
    CHECK(Box.GetSupport(Vector3(-1, 0, 1)) == Vector3(-1, 4, 5));
    KC::Array<Vector3> SupportingFace;
    Box.GetSupportingFace(Vector3::sAxisX(), SupportingFace);
    CHECK(SupportingFace.GetCount() == 4);
    for (const Vector3& Vertex : SupportingFace)
        CHECK(Vertex.GetX() == Box.mMin.GetX());
    Box.GetSupportingFace(-Vector3::sAxisZ(), SupportingFace);
    CHECK(SupportingFace.GetCount() == 4);
    for (const Vector3& Vertex : SupportingFace)
        CHECK(Vertex.GetZ() == Box.mMax.GetZ());

    const AABB Scaled = Box.Scaled(Vector3(-2, 0.5f, 3));
    CHECK(Scaled == AABB(Vector3(-6, -1, -9), Vector3(2, 2, 15)));
    CHECK(Box.Intersect(AABB(Vector3(0, 0, 0), Vector3(4, 5, 6)))
        == AABB(Vector3(0, 0, 0), Vector3(3, 4, 5)));

    AABB Expanded = AABB(Vector3::sZero(), Vector3::sZero());
    Expanded.ExpandBy(Vector3(1, 2, 3));
    CHECK(Expanded == AABB(Vector3(-1, -2, -3), Vector3(1, 2, 3)));
    Expanded.EnsureMinimalEdgeLength(4.0f);
    CHECK(Expanded.GetSize() == Vector3(4, 4, 6));
    Expanded.Translate(Vector3(1, 1, 1));
    CHECK_NEAR(Expanded.GetCenter().GetX(), 2.0f);
    CHECK_NEAR(Expanded.GetCenter().GetY(), 1.0f);
    CHECK_NEAR(Expanded.GetCenter().GetZ(), 1.0f);

    AABB Encapsulated;
    Encapsulated.Encapsulate(Vector3(2, 3, 4));
    Encapsulated.Encapsulate(AABB(Vector3(-1, -2, -3), Vector3(0, 1, 2)));
    CHECK(Encapsulated == AABB(Vector3(-1, -2, -3), Vector3(2, 3, 4)));
    CHECK(AABB(Vector3(1, 2, 3), 2.0f) == AABB(Vector3(-1, 0, 1), Vector3(3, 4, 5)));

    const MG::Plane Ground = MG::Plane::sFromPointAndNormal(Vector3::sZero(), Vector3::sAxisY());
    CHECK(Ground.SignedDistance(Vector3(0, 2, 0)) == 2.0f);
    CHECK(Ground.ProjectPointOnPlane(Vector3(2, 3, 4)) == Vector3(2, 0, 4));
    MV::Float4 StoredPlane;
    Ground.StoreFloat4(&StoredPlane);
    CHECK(StoredPlane == MV::Float4(0.0f, 1.0f, 0.0f, 0.0f));
    CHECK(Ground.Offset(2.0f).SignedDistance(Vector3(0, 2, 0)) == 0.0f);
    CHECK(Ground.GetTransformed(Matrix4::sTranslation(Vector3(0, 3, 0))).SignedDistance(Vector3(0, 3, 0)) == 0.0f);
    CHECK(Box.Overlaps(Ground));
    CHECK(!AABB(Vector3(0, 2, 0), Vector3(1, 3, 1)).Overlaps(Ground));
    Vector3 PlaneIntersection;
    CHECK(MG::Plane::sIntersectPlanes(
        MG::Plane(Vector3::sAxisX(), -1.0f),
        MG::Plane(Vector3::sAxisY(), -2.0f),
        MG::Plane(Vector3::sAxisZ(), -3.0f), PlaneIntersection));
    CHECK(PlaneIntersection == Vector3(1, 2, 3));
    CHECK(!MG::Plane::sIntersectPlanes(
        MG::Plane(Vector3::sAxisX(), 0.0f),
        MG::Plane(Vector3::sAxisX(), 1.0f),
        MG::Plane(Vector3::sAxisZ(), 0.0f), PlaneIntersection));

    MG::Sphere Sphere(Vector3::sZero(), 2.0f);
    CHECK(Sphere.GetSupport(Vector3::sAxisX()) == Vector3(2, 0, 0));
    CHECK(Sphere.Overlaps(MG::Sphere(Vector3(3, 0, 0), 1.0f)));
    CHECK(!Sphere.Overlaps(MG::Sphere(Vector3(3.1f, 0, 0), 1.0f)));
    CHECK(Sphere.Overlaps(AABB(Vector3(2, 0, 0), Vector3(3, 1, 1))));
    CHECK(!Sphere.Overlaps(AABB(Vector3(3, 0, 0), Vector3(4, 1, 1))));
    Sphere.EncapsulatePoint(Vector3(4, 0, 0));
    CHECK(Sphere.GetCenter() == Vector3(1, 0, 0));
    CHECK(Sphere.GetRadius() == 3.0f);

    const MG::OBB Oriented(Matrix4::sIdentity(), Vector3(1, 2, 3));
    CHECK(Oriented.Overlaps(AABB(Vector3(-1, -2, -3), Vector3(1, 2, 3))));
    CHECK(!Oriented.Overlaps(AABB(Vector3(2, 0, 0), Vector3(3, 1, 1))));
    CHECK(Oriented.Overlaps(MG::OBB(Matrix4::sTranslation(Vector3(1, 0, 0)), Vector3(1, 1, 1))));
    CHECK(!Oriented.Overlaps(MG::OBB(Matrix4::sTranslation(Vector3(3.1f, 0, 0)), Vector3(1, 1, 1))));
    const MG::OBB Rotated(Matrix4::sRotationZ(0.78539816339f), Vector3(1, 1, 1));
    CHECK(Rotated.Overlaps(AABB(Vector3(1.2f, 0, -0.5f), Vector3(1.4f, 0.2f, 0.5f))));
    CHECK(!Rotated.Overlaps(AABB(Vector3(1.3f, 1.3f, -0.5f), Vector3(1.5f, 1.5f, 0.5f))));
    CHECK(Rotated.Overlaps(MG::OBB(Matrix4::sTranslation(Vector3(1.2f, 0, 0)), Vector3(1, 1, 1))));
    CHECK(!Rotated.Overlaps(MG::OBB(Matrix4::sTranslation(Vector3(3.0f, 3.0f, 0)), Vector3(1, 1, 1))));

    const MG::Ellipse Ellipse(2.0f, 1.0f);
    CHECK(Ellipse.IsInside(MV::Float2(0, 0)));
    CHECK(!Ellipse.IsInside(MV::Float2(2, 1)));
    const MV::Float2 Closest = Ellipse.GetClosestPoint(MV::Float2(4, 0));
    CHECK_NEAR(Closest.X, 2.0f);
    CHECK_NEAR(Closest.Y, 0.0f);
    const MV::Float2 EllipsePoint = Ellipse.GetClosestPoint(MV::Float2(3, 2));
    CHECK_NEAR(EllipsePoint.X * EllipsePoint.X / 4.0f + EllipsePoint.Y * EllipsePoint.Y, 1.0f);
    CHECK(MG::MortonCode::sExpandBits(0.0f) == 0u);
    CHECK(MG::MortonCode::sExpandBits(1.0f) == 0x09249249u);
    CHECK(MG::MortonCode::sGetMortonCode(Vector3(1, 1, 1), AABB(Vector3::sZero(), Vector3::sOne()))
        == 0x3fffffffu);

    const MG::Triangle Triangle(Vector3(-2, 1, 0), Vector3(2, 1, 0), Vector3(0, 3, 0), 4, 9);
    CHECK_VEC3(Triangle.GetCentroid(), 0.0f, 5.0f / 3.0f, 0.0f);
    AABB TriangleBounds;
    TriangleBounds.Encapsulate(Triangle);
    CHECK(TriangleBounds == AABB(Vector3(-2, 1, 0), Vector3(2, 3, 0)));
    KC::Array<MV::Float3> Vertices;
    Vertices.Add(MV::Float3(-2, 1, 0));
    Vertices.Add(MV::Float3(2, 1, 0));
    Vertices.Add(MV::Float3(0, 3, 0));
    const MG::IndexedTriangle Indexed(1, 2, 0, 4, 9);
    CHECK_VEC3(Indexed.GetCentroid(Vertices), 0.0f, 5.0f / 3.0f, 0.0f);
    CHECK(MG::AABB::sFromTriangle(Vertices, Indexed) == TriangleBounds);
    CHECK(Indexed.GetLowestIndexFirst() == MG::IndexedTriangle(0, 1, 2, 4, 9));
    CHECK(Indexed.IsEquivalent(MG::IndexedTriangle(2, 0, 1, 4, 9)));
    CHECK(Indexed.IsOpposite(MG::IndexedTriangle(1, 0, 2, 4, 9)));
    CHECK(!Indexed.IsDegenerate(Vertices));
    CHECK(Indexed.GetHash() == MG::IndexedTriangle(1, 2, 0, 4, 9).GetHash());

    MG::TriangleList SourceTriangles;
    SourceTriangles.EmplaceBack(Vector3(0, 0, 0), Vector3(1, 0, 0), Vector3(0, 1, 0), 2, 7);
    SourceTriangles.EmplaceBack(Vector3(0.00001f, 0, 0), Vector3(0, 1, 0), Vector3(-1, 0, 0), 3, 8);
    KC::Array<MV::Float3> IndexedVertices;
    MG::IndexedTriangleList IndexedTriangles;
    MG::Indexify(SourceTriangles, IndexedVertices, IndexedTriangles, 0.001f);
    CHECK(IndexedVertices.GetCount() == 4);
    CHECK(IndexedTriangles.GetCount() == 2);
    CHECK(IndexedTriangles[0].mIdx[0] == IndexedTriangles[1].mIdx[0]);
    CHECK(IndexedTriangles[0].mMaterialIndex == 2 && IndexedTriangles[1].mMaterialIndex == 3);
    MG::TriangleList RestoredTriangles;
    MG::Deindexify(IndexedVertices, IndexedTriangles, RestoredTriangles);
    CHECK(RestoredTriangles.GetCount() == 2);
    CHECK(RestoredTriangles[0].mMaterialIndex == 2 && RestoredTriangles[1].mUserData == 8);
    CHECK(RestoredTriangles[0].mVertices[0] == RestoredTriangles[1].mVertices[0]);

    const AABB RayBounds(Vector3(-1, -2, -3), Vector3(1, 2, 3));
    const MG::RayInvDirection RayInverse(Vector3(2, 0, 0));
    CHECK_NEAR(MG::RayAABox(Vector3(-3, 0, 0), RayInverse, RayBounds), 1.0f);
    CHECK_NEAR(MG::RayAABox(Vector3(0, 0, 0), RayInverse, RayBounds), -0.5f);
    const MG::RayInvDirection ParallelRay(Vector3(0, 1, 0));
    CHECK(MG::RayAABox(Vector3(2, 0, 0), ParallelRay, RayBounds) == 3.402823466e38f);
    float RayMinimum = 0.0f;
    float RayMaximum = 0.0f;
    MG::RayAABox(Vector3(-3, 0, 0), RayInverse, RayBounds.mMin, RayBounds.mMax, RayMinimum, RayMaximum);
    CHECK_NEAR(RayMinimum, 1.0f);
    CHECK_NEAR(RayMaximum, 2.0f);
    const Vector4 RayBox4 = MG::RayAABox4(Vector3(-3, 0, 0), RayInverse,
        Vector4(-1, 2, -1, -1), Vector4(-1, 0, 3, -1), Vector4(-1, 0, 0, -1),
        Vector4(1, 3, 2, 1), Vector4(1, 1, 4, 1), Vector4(1, 1, 1, 1));
    CHECK_NEAR(RayBox4.GetX(), 1.0f);
    CHECK_NEAR(RayBox4.GetY(), 2.5f);
    CHECK(RayBox4.GetZ() == 3.402823466e38f);
    CHECK_NEAR(RayBox4.GetW(), 1.0f);
    CHECK_NEAR(MG::RaySphere(Vector3(-3, 0, 0), Vector3(2, 0, 0), Vector3::sZero(), 1.0f), 1.0f);
    CHECK(MG::RaySphere(Vector3(0, 0, 0), Vector3::sAxisX(), Vector3::sZero(), 1.0f) == 0.0f);
    CHECK(MG::RaySphere(Vector3(-3, 3, 0), Vector3::sAxisX(), Vector3::sZero(), 1.0f)
        == 3.402823466e38f);
    CHECK(MG::RaySphere(Vector3(-3, 0, 0), Vector3(2, 0, 0), Vector3::sZero(), 1.0f,
        RayMinimum, RayMaximum) == 2);
    CHECK_NEAR(RayMinimum, 1.0f);
    CHECK_NEAR(RayMaximum, 2.0f);
    CHECK_NEAR(MG::RayTriangle(Vector3(0, 0, 2), Vector3(0, 0, -2),
        Vector3(-1, -1, 0), Vector3(1, -1, 0), Vector3(0, 1, 0)), 1.0f);
    CHECK(MG::RayTriangle(Vector3(2, 2, 2), Vector3(0, 0, -1),
        Vector3(-1, -1, 0), Vector3(1, -1, 0), Vector3(0, 1, 0)) == 3.402823466e38f);
    const Vector4 RayTriangles4 = MG::RayTriangle4(Vector3(0, 0, 2), Vector3(0, 0, -2),
        Vector4(-1, 2, -1, -1), Vector4(-1, 2, -1, -1), Vector4(0, 0, 0, 0),
        Vector4(1, 3, 1, 1), Vector4(-1, 3, -1, -1), Vector4(0, 0, 0, 0),
        Vector4(0, 4, 0, 0), Vector4(1, 4, 1, 1), Vector4(0, 0, 0, 0));
    CHECK_NEAR(RayTriangles4.GetX(), 1.0f);
    CHECK(RayTriangles4.GetY() == 3.402823466e38f);
    CHECK_NEAR(RayTriangles4.GetZ(), 1.0f);
    CHECK_NEAR(RayTriangles4.GetW(), 1.0f);
    CHECK_NEAR(MG::RayCylinder(Vector3(-3, 0, 0), Vector3(2, 0, 0), 1.0f), 1.0f);
    CHECK_NEAR(MG::RayCylinder(Vector3(-3, 2, 0), Vector3(2, -1, 0), 1.0f, 1.0f), 1.0f);
    CHECK(MG::RayCylinder(Vector3(-3, 2, 0), Vector3(2, 0, 0), 1.0f, 1.0f)
        == 3.402823466e38f);
    CHECK(MG::RayCylinder(Vector3(-3, 0, 0), Vector3(2, 0, 0), 1.0f, 1.0f) == 1.0f);
    CHECK_NEAR(MG::RayCapsule(Vector3(-3, 0, 0), Vector3(2, 0, 0), 1.0f, 1.0f), 1.0f);
    CHECK_NEAR(MG::RayCapsule(Vector3(0, 3, 0), Vector3(0, -2, 0), 1.0f, 1.0f), 0.5f);

    KT::UInt32 FeatureSet = 0;
    const Vector3 LinePoint = MG::ClosestPoint::GetClosestPointOnLine(
        Vector3(-1, 0, 0), Vector3(1, 0, 0), FeatureSet);
    CHECK(LinePoint.IsNearZero());
    CHECK(FeatureSet == 0b0011);
    float BarycentricU = 0.0f;
    float BarycentricV = 0.0f;
    CHECK(MG::ClosestPoint::GetBaryCentricCoordinates(
        Vector3(-1, 0, 0), Vector3(1, 0, 0), BarycentricU, BarycentricV));
    CHECK_NEAR(BarycentricU, 0.5f);
    CHECK_NEAR(BarycentricV, 0.5f);
    const Vector3 TrianglePoint = MG::ClosestPoint::GetClosestPointOnTriangle(
        Vector3(1, 0, 0), Vector3(0, 1, 0), Vector3(0, 0, 1), FeatureSet);
    CHECK_VEC3(TrianglePoint, 1.0f / 3.0f, 1.0f / 3.0f, 1.0f / 3.0f);
    CHECK(FeatureSet == 0b0111);
    const Vector3 VertexPoint = MG::ClosestPoint::GetClosestPointOnTriangle(
        Vector3(2, 0, 0), Vector3(3, 0, 0), Vector3(2, 1, 0), FeatureSet);
    CHECK(VertexPoint == Vector3(2, 0, 0));
    CHECK(FeatureSet == 0b0001);
    const Vector3 DegeneratePoint = MG::ClosestPoint::GetClosestPointOnTriangle(
        Vector3(-2, 0, 0), Vector3(2, 0, 0), Vector3(3, 0, 0), FeatureSet);
    CHECK(DegeneratePoint.IsNearZero());
    CHECK(FeatureSet == 0b0101);
    const Vector3 InteriorTetrahedronPoint = MG::ClosestPoint::GetClosestPointOnTetrahedron(
        Vector3(1, 1, 1), Vector3(-1, -1, 1), Vector3(-1, 1, -1), Vector3(1, -1, -1), FeatureSet);
    CHECK(InteriorTetrahedronPoint.IsNearZero());
    CHECK(FeatureSet == 0b1111);
    const Vector3 ExteriorTetrahedronPoint = MG::ClosestPoint::GetClosestPointOnTetrahedron(
        Vector3(1, 0, 0), Vector3(0, 1, 0), Vector3(0, 0, 1), Vector3(1, 1, 1), FeatureSet);
    CHECK_VEC3(ExteriorTetrahedronPoint, 1.0f / 3.0f, 1.0f / 3.0f, 1.0f / 3.0f);
    CHECK(FeatureSet == 0b0111);
    CHECK(MG::ClosestPoint::OriginOutsideOfPlane(
        Vector3(1, 0, 0), Vector3(0, 1, 0), Vector3(0, 0, 1), Vector3(1, 1, 1)));

    KC::Array<Vector3> Polygon;
    Polygon.Add(Vector3(-2, -2, 0));
    Polygon.Add(Vector3(2, -2, 0));
    Polygon.Add(Vector3(2, 2, 0));
    Polygon.Add(Vector3(-2, 2, 0));
    KC::Array<Vector3> ClippedPolygon;
    MG::ClipPolyVsPlane(Polygon, Vector3(-1, 0, 0), Vector3::sAxisX(), ClippedPolygon);
    CHECK(ClippedPolygon.GetCount() == 4);
    for (const Vector3& Vertex : ClippedPolygon)
        CHECK(Vertex.GetX() >= -1.0f);

    KC::Array<Vector3> BoxClippedPolygon;
    MG::ClipPolyVsAABB(Polygon, AABB(Vector3(-1, -1, -1), Vector3(1, 1, 1)), BoxClippedPolygon);
    CHECK(BoxClippedPolygon.GetCount() == 4);
    for (const Vector3& Vertex : BoxClippedPolygon)
        CHECK(AABB(Vector3(-1, -1, -1), Vector3(1, 1, 1)).Contains(Vertex));

    KC::Array<Vector3> ClipSquare;
    ClipSquare.Add(Vector3(-1, -1, 0));
    ClipSquare.Add(Vector3(1, -1, 0));
    ClipSquare.Add(Vector3(1, 1, 0));
    ClipSquare.Add(Vector3(-1, 1, 0));
    KC::Array<Vector3> PolyClippedPolygon;
    MG::ClipPolyVsPoly(Polygon, ClipSquare, Vector3::sAxisZ(), PolyClippedPolygon);
    CHECK(PolyClippedPolygon.GetCount() == 4);

    KC::Array<Vector3> EdgeClippedPolygon;
    MG::ClipPolyVsEdge(
        Polygon,
        Vector3(0, -3, 0),
        Vector3(0, 3, 0),
        Vector3::sAxisZ(),
        EdgeClippedPolygon
    );
    CHECK(EdgeClippedPolygon.GetCount() == 2);
    for (const Vector3& Vertex : EdgeClippedPolygon)
        CHECK_NEAR(Vertex.GetX(), 0.0f);

    const Vector4 BatchMinX(-1, 2, -1, 0);
    const Vector4 BatchMinY(-1, 0, 3, 0);
    const Vector4 BatchMinZ(-1, 0, 0, 0);
    const Vector4 BatchMaxX(1, 3, 2, 1);
    const Vector4 BatchMaxY(1, 1, 4, 1);
    const Vector4 BatchMaxZ(1, 1, 1, 1);
    const UVector4 BatchOverlap = MG::AABB4VsBox(
        AABB(Vector3(-0.5f, -0.5f, -0.5f), Vector3(0.5f, 0.5f, 0.5f)),
        BatchMinX, BatchMinY, BatchMinZ, BatchMaxX, BatchMaxY, BatchMaxZ);
    CHECK(BatchOverlap.GetX() == 0xffffffffu);
    CHECK(BatchOverlap.GetY() == 0u);
    CHECK(BatchOverlap.GetZ() == 0u);
    CHECK(BatchOverlap.GetW() == 0xffffffffu);
    const UVector4 PointOverlap = MG::AABB4VsPoint(
        Vector3(0, 0, 0), BatchMinX, BatchMinY, BatchMinZ, BatchMaxX, BatchMaxY, BatchMaxZ);
    CHECK(PointOverlap.GetX() == 0xffffffffu);
    CHECK(PointOverlap.GetY() == 0u);
    CHECK(PointOverlap.GetZ() == 0u);
    CHECK(PointOverlap.GetW() == 0xffffffffu);
    const Vector4 BatchDistance = MG::AABB4DistanceSqToPoint(
        Vector3::sZero(), BatchMinX, BatchMinY, BatchMinZ, BatchMaxX, BatchMaxY, BatchMaxZ);
    CHECK_NEAR(BatchDistance.GetX(), 0.0f);
    CHECK_NEAR(BatchDistance.GetY(), 4.0f);
    CHECK_NEAR(BatchDistance.GetZ(), 9.0f);
    CHECK_NEAR(BatchDistance.GetW(), 0.0f);
    const UVector4 SphereOverlap = MG::AABB4VsSphere(
        Vector3::sZero(), 5.0f, BatchMinX, BatchMinY, BatchMinZ, BatchMaxX, BatchMaxY, BatchMaxZ);
    CHECK(SphereOverlap.GetX() == 0xffffffffu);
    CHECK(SphereOverlap.GetY() == 0xffffffffu);
    CHECK(SphereOverlap.GetZ() == 0u);
    CHECK(SphereOverlap.GetW() == 0xffffffffu);
    Vector4 ScaledMinX, ScaledMinY, ScaledMinZ, ScaledMaxX, ScaledMaxY, ScaledMaxZ;
    MG::AABB4Scale(Vector3(-2, 1, 1), BatchMinX, BatchMinY, BatchMinZ,
        BatchMaxX, BatchMaxY, BatchMaxZ, ScaledMinX, ScaledMinY, ScaledMinZ,
        ScaledMaxX, ScaledMaxY, ScaledMaxZ);
    CHECK(ScaledMinX == Vector4(-2, -6, -4, -2));
    CHECK(ScaledMaxX == Vector4(2, -4, 2, 0));
    MG::AABB4EnlargeWithExtent(Vector3(1, 2, 3), ScaledMinX, ScaledMinY, ScaledMinZ,
        ScaledMaxX, ScaledMaxY, ScaledMaxZ);
    CHECK(ScaledMinX == Vector4(-3, -7, -5, -3));
    CHECK(ScaledMaxX == Vector4(3, -3, 3, 1));
    const UVector4 OrientedBatch = MG::AABB4VsOBB(
        MG::OBB(Matrix4::sIdentity(), Vector3(1, 1, 1)),
        BatchMinX, BatchMinY, BatchMinZ, BatchMaxX, BatchMaxY, BatchMaxZ);
    CHECK(OrientedBatch.GetX() == 0xffffffffu);
    CHECK(OrientedBatch.GetY() == 0u);
    CHECK(MG::AABB4VsBox(MG::OBB(Matrix4::sIdentity(), Vector3(1, 1, 1)),
        BatchMinX, BatchMinY, BatchMinZ, BatchMaxX, BatchMaxY, BatchMaxZ) == OrientedBatch);

    const AABB Transformed = AABB(Vector3(-1, -2, -3), Vector3(1, 2, 3))
        .Transformed(Matrix4::sTranslation(Vector3(5, -4, 2)) * Matrix4::sRotationZ(1.57079632679f));
    CHECK(Transformed == AABB(Vector3(3, -5, -1), Vector3(7, -3, 5)));

    const auto Axes = KC::StaticArray<Vector3, 6>::Of(
        Vector3::sAxisX(), -Vector3::sAxisX(), Vector3::sAxisY(),
        -Vector3::sAxisY(), Vector3::sAxisZ(), -Vector3::sAxisZ()
    );
    for (const Vector3& Axis : Axes)
    {
        KC::Array<Vector3> Face;
        Box.GetSupportingFace(Axis, Face);
        CHECK(Face.GetCount() == 4);
        const unsigned DominantAxis = Axis.Abs().GetHighestComponentIndex();
        const float Expected = Axis[DominantAxis] < 0.0f ? Box.mMax[DominantAxis] : Box.mMin[DominantAxis];
        for (const Vector3& Point : Face)
            CHECK(Point[DominantAxis] == Expected);
    }

    CHECK(AABB::sBiggest().IsValid());
}
