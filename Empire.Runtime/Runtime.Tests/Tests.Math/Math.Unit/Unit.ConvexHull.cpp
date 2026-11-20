#include "Empire.Runtime/Runtime.Tests/Tests.Math/Math.Unit/Unit.Math.hpp"
#include "Empire.Runtime/Runtime.Tests/Tests.Math/Math.Unit/Unit.Check.hpp"

#include "Empire.Engine/Engine.Kernel/Kernel.Containers/Containers.Array.hpp"
#include "Empire.Engine/Engine.Math/Math.Geometry/Geometry.2D/ConvexHullBuilder2D.hpp"
#include "Empire.Engine/Engine.Math/Math.Geometry/Geometry.3D/ConvexHullBuilder.hpp"

namespace Geometry2D = DEvchik10Empire::EmpireGameEngine::EmpireEngine::EngineMath::Geometry2D;
namespace MathVector = DEvchik10Empire::EmpireGameEngine::EmpireEngine::EngineMath::MathVector;
namespace KernelTypes = DEvchik10Empire::EmpireGameEngine::EmpireEngine::EngineKernel::KernelTypes;
namespace KernelContainers = DEvchik10Empire::EmpireGameEngine::EmpireEngine::EngineKernel::KernelContainers;
namespace MathScalar = DEvchik10Empire::EmpireGameEngine::EmpireEngine::EngineMath::MathScalar;

void TestConvexHull2D()
{
    SECTION("Jolt 2D convex hull builder");

    Geometry2D::ConvexHullBuilder2D::Positions Positions;
    Positions.Add(MathVector::Vector3(-1.0f, -1.0f, 0.0f));
    Positions.Add(MathVector::Vector3(1.0f, -1.0f, 0.0f));
    Positions.Add(MathVector::Vector3(1.0f, 1.0f, 0.0f));
    Positions.Add(MathVector::Vector3(-1.0f, 1.0f, 0.0f));
    Positions.Add(MathVector::Vector3(0.0f, 0.0f, 0.0f));
    Geometry2D::ConvexHullBuilder2D Builder(Positions);
    Geometry2D::ConvexHullBuilder2D::Edges Hull;

    CHECK(Builder.Initialize(0, 1, 2, 8, 1.0e-4f, Hull)
        == Geometry2D::ConvexHullBuilder2D::Result::Success);
    CHECK(Hull.GetCount() == 4);
    KernelTypes::Float SignedAreaTwice = 0.0f;
    KernelTypes::Bool ContainsAllCorners = true;
    for (KernelTypes::Size Index = 0; Index < Hull.GetCount(); ++Index)
    {
        const auto CurrentIndex = static_cast<KernelTypes::Size>(Hull[Index]);
        const auto NextIndex = static_cast<KernelTypes::Size>(Hull[(Index + 1) % Hull.GetCount()]);
        const MathVector::Vector3& Current = Positions[CurrentIndex];
        const MathVector::Vector3& Next = Positions[NextIndex];
        SignedAreaTwice += Current.GetX() * Next.GetY() - Current.GetY() * Next.GetX();
        ContainsAllCorners = ContainsAllCorners && Hull.Find(static_cast<KernelTypes::Int32>(Index)) != Hull.end();
    }

    CHECK(SignedAreaTwice > 0.0f);
    CHECK(ContainsAllCorners);

    CHECK(Builder.Initialize(0, 1, 2, 3, 1.0e-4f, Hull)
        == Geometry2D::ConvexHullBuilder2D::Result::MaxVerticesReached);
    CHECK(Hull.GetCount() == 3);

    CHECK(Builder.Initialize(0, 1, 2, 8, 4.0f, Hull)
        == Geometry2D::ConvexHullBuilder2D::Result::Success);
    CHECK(Hull.GetCount() == 3);

    Geometry2D::ConvexHullBuilder2D::Positions TrianglePositions;
    TrianglePositions.Add(MathVector::Vector3(-1.0f, -1.0f, 0.0f));
    TrianglePositions.Add(MathVector::Vector3(1.0f, -1.0f, 0.0f));
    TrianglePositions.Add(MathVector::Vector3(-1.0f, 1.0f, 0.0f));
    Geometry2D::ConvexHullBuilder2D TriangleBuilder(TrianglePositions);
    CHECK(TriangleBuilder.Initialize(0, 1, 2, 8, 1.0e-4f, Hull)
        == Geometry2D::ConvexHullBuilder2D::Result::Success);
    CHECK(Hull.GetCount() == 3);

    Geometry2D::ConvexHullBuilder2D::Positions Grid;
    for (KernelTypes::Int32 Y = -2; Y <= 2; ++Y)
        for (KernelTypes::Int32 X = -2; X <= 2; ++X)
            Grid.Add(MathVector::Vector3(static_cast<float>(X), static_cast<float>(Y), 0.0f));
    Geometry2D::ConvexHullBuilder2D GridBuilder(Grid);
    CHECK(GridBuilder.Initialize(0, 4, 24, 8, 1.0e-4f, Hull)
        == Geometry2D::ConvexHullBuilder2D::Result::Success);
    CHECK(Hull.GetCount() == 4);
    CHECK(Hull.Find(0) != Hull.end() && Hull.Find(4) != Hull.end());
    CHECK(Hull.Find(20) != Hull.end() && Hull.Find(24) != Hull.end());

    Geometry2D::ConvexHullBuilder2D::Positions LargeGrid;
    for (KernelTypes::Int32 Y = 0; Y < 10; ++Y)
        for (KernelTypes::Int32 X = 0; X < 10; ++X)
            LargeGrid.Add(MathVector::Vector3(0.1f * X, 0.2f * Y, 0.0f));
    Geometry2D::ConvexHullBuilder2D LargeGridBuilder(LargeGrid);
    CHECK(LargeGridBuilder.Initialize(0, 9, 99, 8, 1.0e-4f, Hull)
        == Geometry2D::ConvexHullBuilder2D::Result::Success);
    CHECK(Hull.GetCount() == 4);
    CHECK(Hull.Find(0) != Hull.end() && Hull.Find(9) != Hull.end());
    CHECK(Hull.Find(99) != Hull.end() && Hull.Find(90) != Hull.end());
}

void TestConvexHull3D()
{
    SECTION("Jolt 3D convex hull builder");
    namespace Geometry3D = DEvchik10Empire::EmpireGameEngine::EmpireEngine::EngineMath::Geometry3D;

    Geometry3D::ConvexHullBuilder::Positions Tetrahedron;
    Tetrahedron.Add(MathVector::Vector3(0.0f, 0.0f, 0.0f));
    Tetrahedron.Add(MathVector::Vector3(1.0f, 0.0f, 0.0f));
    Tetrahedron.Add(MathVector::Vector3(0.0f, 1.0f, 0.0f));
    Tetrahedron.Add(MathVector::Vector3(0.0f, 0.0f, 1.0f));
    Geometry3D::ConvexHullBuilder Builder(Tetrahedron);
    const char* Error = nullptr;
    CHECK(Builder.Initialize(32, 1.0e-4f, Error) == Geometry3D::ConvexHullBuilder::Result::Success);
    CHECK(Error == nullptr);
    CHECK(Builder.GetNumVerticesUsed() == 4);
    CHECK(Builder.GetFaces().GetCount() == 4);
    KernelContainers::Array<KernelTypes::Int32> TetrahedronFace;
    TetrahedronFace.Add(1);
    TetrahedronFace.Add(2);
    TetrahedronFace.Add(3);
    CHECK(Builder.ContainsFace(TetrahedronFace));
    CHECK(!Builder.ContainsFace(KernelContainers::Array<KernelTypes::Int32>{}));

    Geometry3D::ConvexHullBuilder::Positions Cube;
    for (KernelTypes::Int32 Z = 0; Z <= 1; ++Z)
        for (KernelTypes::Int32 Y = 0; Y <= 1; ++Y)
            for (KernelTypes::Int32 X = 0; X <= 1; ++X)
                Cube.Add(MathVector::Vector3(static_cast<float>(X), static_cast<float>(Y), static_cast<float>(Z)));
    Cube.Add(MathVector::Vector3(0.5f, 0.5f, 0.5f));
    Geometry3D::ConvexHullBuilder CubeBuilder(Cube);
    CHECK(CubeBuilder.Initialize(32, 1.0e-4f, Error) == Geometry3D::ConvexHullBuilder::Result::Success);
    CHECK(CubeBuilder.GetNumVerticesUsed() == 8);
    CHECK(CubeBuilder.GetFaces().GetCount() == 6);
    KernelContainers::Array<KernelTypes::Int32> CubeFace;
    CubeFace.Add(0);
    CubeFace.Add(4);
    CubeFace.Add(6);
    CubeFace.Add(2);
    CHECK(CubeBuilder.ContainsFace(CubeFace));
    MathVector::Vector3 CenterOfMass;
    KernelTypes::Float Volume = 0.0f;
    CubeBuilder.GetCenterOfMassAndVolume(CenterOfMass, Volume);
    CHECK_NEAR(Volume, 1.0f);
    CHECK_VEC3(CenterOfMass, 0.5f, 0.5f, 0.5f);

    Geometry3D::ConvexHullBuilder::Positions DenseCube;
    for (KernelTypes::Int32 X = 0; X < 10; ++X)
        for (KernelTypes::Int32 Y = 0; Y < 10; ++Y)
            for (KernelTypes::Int32 Z = 0; Z < 10; ++Z)
                DenseCube.Add(MathVector::Vector3(0.1f * X, 1.0f + 0.2f * Y, 0.3f * Z));
    Geometry3D::ConvexHullBuilder DenseCubeBuilder(DenseCube);
    CHECK(DenseCubeBuilder.Initialize(2147483647, 1.0e-3f, Error)
        == Geometry3D::ConvexHullBuilder::Result::Success);
    CHECK(DenseCubeBuilder.GetNumVerticesUsed() == 8);
    CHECK(DenseCubeBuilder.GetFaces().GetCount() == 6);
    const auto CheckCubeFace = [&DenseCubeBuilder](KernelTypes::Int32 A, KernelTypes::Int32 B,
                                      KernelTypes::Int32 C, KernelTypes::Int32 D)
    {
        KernelContainers::Array<KernelTypes::Int32> Face;
        Face.Add(A);
        Face.Add(B);
        Face.Add(C);
        Face.Add(D);
        CHECK(DenseCubeBuilder.ContainsFace(Face));
    };
    CheckCubeFace(0, 9, 99, 90);
    CheckCubeFace(0, 90, 990, 900);
    CheckCubeFace(900, 990, 999, 909);
    CheckCubeFace(9, 909, 999, 99);
    CheckCubeFace(90, 99, 999, 990);
    CheckCubeFace(0, 900, 909, 9);

    Geometry3D::ConvexHullBuilder::Positions PlanarQuad;
    PlanarQuad.Add(MathVector::Vector3(-1.0f, 0.0f, -1.0f));
    PlanarQuad.Add(MathVector::Vector3(1.0f, 0.0f, -1.0f));
    PlanarQuad.Add(MathVector::Vector3(-1.0f, 0.0f, 1.0f));
    PlanarQuad.Add(MathVector::Vector3(1.0f, 0.0f, 1.0f));
    PlanarQuad.Add(MathVector::Vector3(0.0f, 0.0f, 0.0f));
    Geometry3D::ConvexHullBuilder PlanarQuadBuilder(PlanarQuad);
    CHECK(PlanarQuadBuilder.Initialize(32, 1.0e-4f, Error)
        == Geometry3D::ConvexHullBuilder::Result::Success);
    CHECK(PlanarQuadBuilder.GetNumVerticesUsed() == 4);
    CHECK(PlanarQuadBuilder.GetFaces().GetCount() == 2);
    KernelContainers::Array<KernelTypes::Int32> PlanarFace;
    PlanarFace.Add(0);
    PlanarFace.Add(1);
    PlanarFace.Add(3);
    PlanarFace.Add(2);
    CHECK(PlanarQuadBuilder.ContainsFace(PlanarFace));
    PlanarFace.Clear();
    PlanarFace.Add(2);
    PlanarFace.Add(3);
    PlanarFace.Add(1);
    PlanarFace.Add(0);
    CHECK(PlanarQuadBuilder.ContainsFace(PlanarFace));

    Geometry3D::ConvexHullBuilder LimitedBuilder(Cube);
    CHECK(LimitedBuilder.Initialize(4, 1.0e-4f, Error)
        == Geometry3D::ConvexHullBuilder::Result::MaxVerticesReached);
    CHECK(LimitedBuilder.GetNumVerticesUsed() >= 4);

    Geometry3D::ConvexHullBuilder::Positions Collinear;
    Collinear.Add(MathVector::Vector3(0.0f, 0.0f, 0.0f));
    Collinear.Add(MathVector::Vector3(1.0f, 0.0f, 0.0f));
    Collinear.Add(MathVector::Vector3(2.0f, 0.0f, 0.0f));
    Geometry3D::ConvexHullBuilder DegenerateBuilder(Collinear);
    CHECK(DegenerateBuilder.Initialize(32, 1.0e-4f, Error)
        == Geometry3D::ConvexHullBuilder::Result::Degenerate);

    Geometry3D::ConvexHullBuilder::Positions Sphere;
    for (KernelTypes::Int32 Radius = 0; Radius < 10; ++Radius)
        for (KernelTypes::Int32 Phi = 0; Phi < 10; ++Phi)
            for (KernelTypes::Int32 Theta = 0; Theta < 10; ++Theta)
                Sphere.Add(static_cast<float>(2 * Radius) * (Theta == 0
                    ? MathVector::Vector3::sAxisZ()
                    : Theta == 9
                        ? -MathVector::Vector3::sAxisZ()
                        : MathVector::Vector3::sUnitSpherical(
                            MathScalar::Pi * Theta / 9.0f,
                            MathScalar::TwoPi * Phi / 10.0f)));
    Geometry3D::ConvexHullBuilder SphereBuilder(Sphere);
    CHECK(SphereBuilder.Initialize(2147483647, 1.0e-3f, Error)
        == Geometry3D::ConvexHullBuilder::Result::Success);
    CHECK(SphereBuilder.GetNumVerticesUsed() == 82);
    Geometry3D::ConvexHullBuilder::Face* SphereErrorFace = nullptr;
    KernelTypes::Float SphereMaximumError = 0.0f;
    KernelTypes::Int32 SphereErrorPosition = -1;
    KernelTypes::Float SphereCoplanarDistance = 0.0f;
    SphereBuilder.DetermineMaxError(
        SphereErrorFace, SphereMaximumError, SphereErrorPosition, SphereCoplanarDistance);
    CHECK(SphereMaximumError < MathScalar::Max(SphereCoplanarDistance, 1.0e-3f));

    const auto CheckAccurateHull = [&Error](const Geometry3D::ConvexHullBuilder::Positions& Positions)
    {
        Geometry3D::ConvexHullBuilder EdgeCaseBuilder(Positions);
        CHECK(EdgeCaseBuilder.Initialize(2147483647, 1.0e-3f, Error)
            == Geometry3D::ConvexHullBuilder::Result::Success);
        Geometry3D::ConvexHullBuilder::Face* ErrorFace = nullptr;
        KernelTypes::Float MaximumError = 0.0f;
        KernelTypes::Int32 ErrorPosition = -1;
        KernelTypes::Float CoplanarDistance = 0.0f;
        EdgeCaseBuilder.DetermineMaxError(ErrorFace, MaximumError, ErrorPosition, CoplanarDistance);
        CHECK(MaximumError < MathScalar::Max(CoplanarDistance, 1.2e-3f));
    };

    Geometry3D::ConvexHullBuilder::Positions NearlyCoplanarHull;
    NearlyCoplanarHull.Add(MathVector::Vector3(-0.020472288f, -0.195635557f, 0.308015466f));
    NearlyCoplanarHull.Add(MathVector::Vector3(0.136248738f, 0.633286834f, 0.135366619f));
    NearlyCoplanarHull.Add(MathVector::Vector3(0.286418647f, -0.228475571f, 0.308084548f));
    NearlyCoplanarHull.Add(MathVector::Vector3(-0.267285109f, 1.024676085f, 0.308042824f));
    NearlyCoplanarHull.Add(MathVector::Vector3(0.396568149f, -0.971658647f, 0.308055162f));
    NearlyCoplanarHull.Add(MathVector::Vector3(0.321081549f, -1.024676085f, 0.308036327f));
    NearlyCoplanarHull.Add(MathVector::Vector3(0.034643859f, -0.404506862f, 0.308015764f));
    NearlyCoplanarHull.Add(MathVector::Vector3(0.189224690f, -0.252762139f, 0.308060408f));
    CheckAccurateHull(NearlyCoplanarHull);

    Geometry3D::ConvexHullBuilder::Positions ThinPlanarHull;
    ThinPlanarHull.Add(MathVector::Vector3(0.917345762f, 0.157111734f, 1.650970459f));
    ThinPlanarHull.Add(MathVector::Vector3(-0.098074198f, 0.157116055f, 0.664742708f));
    ThinPlanarHull.Add(MathVector::Vector3(1.777100325f, 0.157112047f, 1.238879442f));
    ThinPlanarHull.Add(MathVector::Vector3(2.114324570f, 0.157112464f, 0.780688763f));
    ThinPlanarHull.Add(MathVector::Vector3(1.926570415f, 0.157114446f, 0.240761161f));
    ThinPlanarHull.Add(MathVector::Vector3(-1.045998096f, 0.157108605f, 1.548911095f));
    ThinPlanarHull.Add(MathVector::Vector3(-1.820045233f, 0.157106474f, 1.050360918f));
    ThinPlanarHull.Add(MathVector::Vector3(-1.918573976f, 0.157108605f, 0.039246202f));
    ThinPlanarHull.Add(MathVector::Vector3(0.042619467f, 0.157113969f, -1.405336142f));
    ThinPlanarHull.Add(MathVector::Vector3(0.575986624f, 0.157114401f, -1.370834589f));
    ThinPlanarHull.Add(MathVector::Vector3(1.402592659f, 0.157115221f, -0.834864557f));
    ThinPlanarHull.Add(MathVector::Vector3(1.110557318f, 0.157113969f, -1.336267948f));
    ThinPlanarHull.Add(MathVector::Vector3(1.689781666f, 0.157115355f, -0.308773756f));
    ThinPlanarHull.Add(MathVector::Vector3(2.205337524f, 0.157113209f, -0.281754494f));
    ThinPlanarHull.Add(MathVector::Vector3(-1.346967936f, 0.157110974f, -0.978962541f));
    ThinPlanarHull.Add(MathVector::Vector3(-1.346967936f, 0.157110974f, -0.978962541f));
    ThinPlanarHull.Add(MathVector::Vector3(-2.085033417f, 0.157106936f, -0.506602883f));
    ThinPlanarHull.Add(MathVector::Vector3(-0.981224537f, 0.157110706f, -1.445893764f));
    ThinPlanarHull.Add(MathVector::Vector3(-0.481085658f, 0.157112658f, -1.426232934f));
    ThinPlanarHull.Add(MathVector::Vector3(-0.981224537f, 0.157110706f, -1.445893764f));
    CheckAccurateHull(ThinPlanarHull);

    MathTests::Random Random(0x1ee7c0de);
    for (KernelTypes::Int32 Iteration = 0; Iteration < 10; ++Iteration)
    {
        Geometry3D::ConvexHullBuilder::Positions RandomPositions;
        for (KernelTypes::Int32 Index = 0; Index < 64; ++Index)
            RandomPositions.Add(MathVector::Vector3(
                Random.Range(-1.0f, 1.0f),
                Random.Range(-1.0f, 1.0f),
                Random.Range(-1.0f, 1.0f)
            ));

        Geometry3D::ConvexHullBuilder RandomBuilder(RandomPositions);
        CHECK(RandomBuilder.Initialize(256, 1.0e-3f, Error)
            == Geometry3D::ConvexHullBuilder::Result::Success);
        Geometry3D::ConvexHullBuilder::Face* ErrorFace = nullptr;
        KernelTypes::Float MaximumError = 0.0f;
        KernelTypes::Int32 ErrorPosition = -1;
        KernelTypes::Float CoplanarDistance = 0.0f;
        RandomBuilder.DetermineMaxError(ErrorFace, MaximumError, ErrorPosition, CoplanarDistance);
        CHECK(MaximumError <= MathScalar::Max(CoplanarDistance, 1.2e-3f));
    }

    const auto RandomDirection = [&Random]()
    {
        MathVector::Vector3 Direction;
        do
        {
            Direction = MathVector::Vector3(
                Random.Range(-1.0f, 1.0f),
                Random.Range(-1.0f, 1.0f),
                Random.Range(-1.0f, 1.0f));
        } while (Direction.LengthSq() < 1.0e-6f);
        return Direction.Normalized();
    };
    for (KernelTypes::Int32 Iteration = 0; Iteration < 20; ++Iteration)
    {
        const KernelTypes::Float RadiusStart = Random.Range(0.1f, 0.5f);
        const KernelTypes::Float RadiusRange = Random.Range(0.1f, 2.0f);
        const MathVector::Vector3 ShapeScale(
            Random.Range(0.1f, 1.0f),
            Random.Range(0.1f, 1.0f),
            Random.Range(0.1f, 1.0f));
        Geometry3D::ConvexHullBuilder::Positions RandomPositions;
        for (KernelTypes::Int32 Index = 0; Index < 100; ++Index)
        {
            const MathVector::Vector3 Point = Random.Range(RadiusStart, RadiusStart + RadiusRange)
                * RandomDirection() * ShapeScale;
            RandomPositions.Add(Point);
            RandomPositions.Add(Point + 1.0e-3f * Random.Range(0.0f, 2.0f) * RandomDirection());

            const KernelTypes::Float Fraction = Random.Range(0.0f, 1.0f);
            const MathVector::Vector3 InterpolatedPoint = Fraction * Point
                + (1.0f - Fraction) * RandomPositions[
                    static_cast<KernelTypes::Size>(Random.Next() % RandomPositions.GetCount())];
            RandomPositions.Add(InterpolatedPoint);
            RandomPositions.Add(
                InterpolatedPoint + 1.0e-3f * Random.Range(0.0f, 2.0f) * RandomDirection());
        }

        Geometry3D::ConvexHullBuilder RandomBuilder(RandomPositions);
        CHECK(RandomBuilder.Initialize(2147483647, 1.0e-3f, Error)
            == Geometry3D::ConvexHullBuilder::Result::Success);
        Geometry3D::ConvexHullBuilder::Face* ErrorFace = nullptr;
        KernelTypes::Float MaximumError = 0.0f;
        KernelTypes::Int32 ErrorPosition = -1;
        KernelTypes::Float CoplanarDistance = 0.0f;
        RandomBuilder.DetermineMaxError(ErrorFace, MaximumError, ErrorPosition, CoplanarDistance);
        if (MaximumError > MathScalar::Max(CoplanarDistance, 1.2e-3f))
            std::printf("  noisy hull %d: error=%g position=%d normal=(%g,%g,%g)\n", Iteration,
                MaximumError, ErrorPosition, ErrorFace->Normal.GetX(), ErrorFace->Normal.GetY(),
                ErrorFace->Normal.GetZ());
        CHECK(MaximumError <= MathScalar::Max(CoplanarDistance, 1.2e-3f));
    }
}
