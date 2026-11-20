#include "Empire.Engine/Engine.Math/Math.Geometry/Geometry.3D/ConvexHullBuilder.hpp"

#include <cfloat>
#include <cstdint>

#include "Empire.Engine/Engine.Kernel/Kernel.Assert/Assert.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Compiler/Compiler.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Memory/Memory.hpp"
#include "Empire.Engine/Engine.Math/Math.Geometry/Geometry.2D/ConvexHullBuilder2D.hpp"
#include "Empire.Engine/Engine.Math/Math.Geometry/Geometry.3D/ClosestPoint.hpp"
#include "Empire.Engine/Engine.Math/Math.Scalar/Scalar.hpp"

// Порт Jolt/Geometry/ConvexHullBuilder.cpp (Jolt 5.7.0): quickhull со списками конфликтов,
// слиянием компланарных/вырожденных граней и удалением некорректных рёбер.
// Отладочная отрисовка (JPH_CONVEX_BUILDER_DEBUG) и дамп формы (JPH_CONVEX_BUILDER_DUMP_SHAPE) не переносятся.

namespace DEvchik10Empire
{
    namespace EmpireGameEngine
    {
        namespace EmpireEngine
        {
            namespace EngineMath
            {
                namespace Geometry3D
                {
                    namespace
                    {
                        using KernelTypes::Float;
                        using KernelTypes::Int32;
                        using KernelTypes::Size;

                        constexpr Float FloatEpsilon = FLT_EPSILON;

                        [[nodiscard]] inline Float Square(Float Value) noexcept
                        {
                            return Value * Value;
                        }

                        [[nodiscard]] inline Float Maximum(Float A, Float B) noexcept
                        {
                            return A > B ? A : B;
                        }

                        [[nodiscard]] inline Float Minimum(Float A, Float B) noexcept
                        {
                            return A < B ? A : B;
                        }

                        [[nodiscard]] inline Float Absolute(Float Value) noexcept
                        {
                            return Value < 0.0f ? -Value : Value;
                        }
                    }

                    ConvexHullBuilder::Edge* ConvexHullBuilder::Edge::GetPreviousEdge() noexcept
                    {
                        Edge* Previous = this;
                        while (Previous->NextEdge != this)
                            Previous = Previous->NextEdge;
                        return Previous;
                    }

                    ConvexHullBuilder::Face::~Face()
                    {
                        Edge* Current = FirstEdge;
                        if (Current != nullptr)
                        {
                            do
                            {
                                Edge* Next = Current->NextEdge;
                                KernelMemory::Destroy(Current);
                                Current = Next;
                            } while (Current != FirstEdge);
                        }
                    }

                    void ConvexHullBuilder::Face::CalculateNormalAndCentroid(const Vector3* Positions)
                    {
                        // Точка, от которой строится веер треугольников
                        Edge* Current = FirstEdge;
                        const Vector3 Y0 = Positions[Current->StartIndex];

                        Current = Current->NextEdge;
                        Vector3 Y1 = Positions[Current->StartIndex];

                        Centroid = Y0 + Y1;
                        Int32 Count = 2;
                        Normal = Vector3::sZero();

                        for (Current = Current->NextEdge; Current != FirstEdge; Current = Current->NextEdge)
                        {
                            const Vector3 Y2 = Positions[Current->StartIndex];

                            const Vector3 E0 = Y1 - Y0;
                            const Vector3 E1 = Y2 - Y1;
                            const Vector3 E2 = Y0 - Y2;

                            // Лучшая нормаль считается по двум самым коротким рёбрам
                            // (https://box2d.org/posts/2014/01/troublesome-triangle/): выбираем более короткое из E1 и E2.
                            if (E1.LengthSq() < E2.LengthSq())
                                Normal += E0.Cross(E1);
                            else
                                Normal += E2.Cross(E0);

                            Centroid += Y2;
                            ++Count;
                            Y1 = Y2;
                        }

                        Centroid /= static_cast<Float>(Count);
                    }

                    void ConvexHullBuilder::Face::Initialize(
                        Int32 Index0, Int32 Index1, Int32 Index2, const Vector3* PositionValues)
                    {
                        KernelAssert::DebugCheck(FirstEdge == nullptr, "ConvexHullBuilder::Face::Initialize: грань уже инициализирована");
                        KernelAssert::DebugCheck(Index0 != Index1 && Index0 != Index2 && Index1 != Index2,
                            "ConvexHullBuilder::Face::Initialize: индексы должны быть различны");

                        Edge* E0 = KernelMemory::Make<Edge>(this, Index0);
                        Edge* E1 = KernelMemory::Make<Edge>(this, Index1);
                        Edge* E2 = KernelMemory::Make<Edge>(this, Index2);

                        E0->NextEdge = E1;
                        E1->NextEdge = E2;
                        E2->NextEdge = E0;
                        FirstEdge = E0;

                        CalculateNormalAndCentroid(PositionValues);
                    }

                    KernelTypes::Bool ConvexHullBuilder::Face::IsFacing(const Vector3& Position) const noexcept
                    {
                        KernelAssert::DebugCheck(!Removed, "ConvexHullBuilder::Face::IsFacing: грань удалена");
                        return Normal.Dot(Position - Centroid) > 0.0f;
                    }

                    ConvexHullBuilder::~ConvexHullBuilder()
                    {
                        ClearFaces();
                    }

                    void ConvexHullBuilder::ClearFaces() noexcept
                    {
                        for (Face* FaceValue : HullFaces)
                            KernelMemory::Destroy(FaceValue);
                        HullFaces.Clear();
                    }

                    void ConvexHullBuilder::GetFaceForPoint(
                        const Vector3& Point, const Faces& FaceList, Face*& OutFace, Float& OutDistanceSquared) const
                    {
                        OutFace = nullptr;
                        OutDistanceSquared = 0.0f;

                        for (Face* FaceValue : FaceList)
                        {
                            if (FaceValue->Removed)
                                continue;

                            const Float Dot = FaceValue->Normal.Dot(Point - FaceValue->Centroid);
                            if (Dot > 0.0f)
                            {
                                const Float DistanceSquared = Dot * Dot / FaceValue->Normal.LengthSq();
                                if (DistanceSquared > OutDistanceSquared)
                                {
                                    OutFace = FaceValue;
                                    OutDistanceSquared = DistanceSquared;
                                }
                            }
                        }
                    }

                    Float ConvexHullBuilder::GetDistanceToEdgeSquared(const Vector3& Point, const Face* FaceValue) const
                    {
                        KernelTypes::Bool AllInside = true;
                        Float EdgeDistanceSquared = FLT_MAX;

                        const Edge* EdgeValue = FaceValue->FirstEdge;
                        Vector3 P1 = PositionsReference[static_cast<Size>(
                            const_cast<Edge*>(EdgeValue)->GetPreviousEdge()->StartIndex)];
                        do
                        {
                            const Vector3 P2 = PositionsReference[static_cast<Size>(EdgeValue->StartIndex)];
                            if ((P2 - P1).Cross(Point - P1).Dot(FaceValue->Normal) < 0.0f)
                            {
                                // Точка снаружи этого ребра: измеряем расстояние до него
                                AllInside = false;
                                KernelTypes::UInt32 FeatureSet = 0;
                                EdgeDistanceSquared = Minimum(EdgeDistanceSquared,
                                    ClosestPoint::GetClosestPointOnLine(P1 - Point, P2 - Point, FeatureSet).LengthSq());
                            }
                            P1 = P2;
                            EdgeValue = EdgeValue->NextEdge;
                        } while (EdgeValue != FaceValue->FirstEdge);

                        return AllInside ? 0.0f : EdgeDistanceSquared;
                    }

                    KernelTypes::Bool ConvexHullBuilder::AssignPointToFace(
                        Int32 PositionIndex, const Faces& FaceList, Float ToleranceSquared)
                    {
                        const Vector3 Point = PositionsReference[static_cast<Size>(PositionIndex)];

                        Face* BestFace = nullptr;
                        Float BestDistanceSquared = 0.0f;
                        GetFaceForPoint(Point, FaceList, BestFace, BestDistanceSquared);

                        if (BestFace != nullptr)
                        {
                            if (BestDistanceSquared <= ToleranceSquared)
                            {
                                // Точка в пределах допуска от плоскости: проверяем расстояние до рёбер
                                const Float DistanceToEdgeSquared = GetDistanceToEdgeSquared(Point, BestFace);
                                if (DistanceToEdgeSquared > ToleranceSquared)
                                {
                                    // Снаружи грани и слишком далеко, чтобы отбросить
                                    CoplanarPositions.Add(Coplanar{PositionIndex, DistanceToEdgeSquared});
                                }
                            }
                            else
                            {
                                // Точка перед гранью: в список конфликтов
                                if (BestDistanceSquared > BestFace->FurthestPointDistanceSquared
                                    || BestFace->ConflictList.IsEmpty())
                                {
                                    // Самая дальняя точка идёт в конец списка
                                    BestFace->FurthestPointDistanceSquared = BestDistanceSquared;
                                    BestFace->ConflictList.Add(PositionIndex);
                                }
                                else
                                {
                                    // Не самая дальняя: вставляем перед последней
                                    BestFace->ConflictList.Insert(BestFace->ConflictList.GetCount() - 1, PositionIndex);
                                }

                                return true;
                            }
                        }

                        return false;
                    }

                    Float ConvexHullBuilder::DetermineCoplanarDistance() const noexcept
                    {
                        // Формула из Implementing Quickhull (Dirk Gregorius)
                        Vector3 Maximum = Vector3::sZero();
                        for (const Vector3& Position : PositionsReference)
                            Maximum = Vector3::sMax(Maximum, Position.Abs());
                        return 3.0f * FloatEpsilon * (Maximum.GetX() + Maximum.GetY() + Maximum.GetZ());
                    }

                    Int32 ConvexHullBuilder::GetNumVerticesUsed() const
                    {
                        KernelContainers::Array<KernelTypes::Bool> Used;
                        Used.Resize(PositionsReference.GetCount(), false);
                        Int32 Count = 0;
                        for (const Face* FaceValue : HullFaces)
                        {
                            const Edge* EdgeValue = FaceValue->FirstEdge;
                            do
                            {
                                const Size Index = static_cast<Size>(EdgeValue->StartIndex);
                                if (!Used[Index])
                                {
                                    Used[Index] = true;
                                    ++Count;
                                }
                                EdgeValue = EdgeValue->NextEdge;
                            } while (EdgeValue != FaceValue->FirstEdge);
                        }
                        return Count;
                    }

                    KernelTypes::Bool ConvexHullBuilder::ContainsFace(const KernelContainers::Array<Int32>& Indices) const
                    {
                        if (Indices.IsEmpty())
                            return false;

                        for (const Face* FaceValue : HullFaces)
                        {
                            const Edge* EdgeValue = FaceValue->FirstEdge;
                            const Int32* Index = Indices.Find(EdgeValue->StartIndex);
                            if (Index != nullptr)
                            {
                                Size Matches = 0;
                                do
                                {
                                    if (*Index != EdgeValue->StartIndex)
                                        break;

                                    ++Matches;

                                    ++Index;
                                    if (Index == Indices.end())
                                        Index = Indices.begin();

                                    EdgeValue = EdgeValue->NextEdge;
                                } while (EdgeValue != FaceValue->FirstEdge);

                                if (Matches == Indices.GetCount())
                                    return true;
                            }
                        }

                        return false;
                    }

                    ConvexHullBuilder::Result ConvexHullBuilder::Initialize(
                        Int32 MaxVertices, Float Tolerance, const char*& Error)
                    {
                        // Освобождаем грани предыдущего построения
                        ClearFaces();
                        CoplanarPositions.Clear();
                        Error = nullptr;

                        const Int32 PositionCount = static_cast<Int32>(PositionsReference.GetCount());
                        if (PositionCount < 3)
                        {
                            Error = "Need at least 3 points to make a hull";
                            return Result::TooFewPoints;
                        }

                        // Допуск для определения компланарности
                        const Float CoplanarToleranceSquared = Square(DetermineCoplanarDistance());

                        // Увеличиваем допуск, если точность не позволяет заданный
                        const Float ToleranceSquared = Maximum(CoplanarToleranceSquared, Square(Tolerance));

                        // Точка, самая далёкая от начала координат
                        Int32 Index1 = -1;
                        Float MaxDistanceSquared = -1.0f;
                        for (Int32 Index = 0; Index < PositionCount; ++Index)
                        {
                            const Float DistanceSquared = PositionsReference[static_cast<Size>(Index)].LengthSq();
                            if (DistanceSquared > MaxDistanceSquared)
                            {
                                MaxDistanceSquared = DistanceSquared;
                                Index1 = Index;
                            }
                        }
                        KernelAssert::DebugCheck(Index1 >= 0, "ConvexHullBuilder::Initialize: не найдена первая точка");

                        // Точка, самая далёкая от первой
                        Int32 Index2 = -1;
                        MaxDistanceSquared = -1.0f;
                        for (Int32 Index = 0; Index < PositionCount; ++Index)
                        {
                            if (Index == Index1)
                                continue;
                            const Float DistanceSquared = (PositionsReference[static_cast<Size>(Index)]
                                - PositionsReference[static_cast<Size>(Index1)]).LengthSq();
                            if (DistanceSquared > MaxDistanceSquared)
                            {
                                MaxDistanceSquared = DistanceSquared;
                                Index2 = Index;
                            }
                        }
                        KernelAssert::DebugCheck(Index2 >= 0, "ConvexHullBuilder::Initialize: не найдена вторая точка");

                        // Точка, образующая наибольший треугольник
                        Int32 Index3 = -1;
                        Float BestTriangleAreaSquared = -1.0f;
                        for (Int32 Index = 0; Index < PositionCount; ++Index)
                        {
                            if (Index == Index1 || Index == Index2)
                                continue;
                            const Vector3& Point = PositionsReference[static_cast<Size>(Index)];
                            const Float TriangleAreaSquared = (PositionsReference[static_cast<Size>(Index1)] - Point)
                                .Cross(PositionsReference[static_cast<Size>(Index2)] - Point).LengthSq();
                            if (TriangleAreaSquared > BestTriangleAreaSquared)
                            {
                                BestTriangleAreaSquared = TriangleAreaSquared;
                                Index3 = Index;
                            }
                        }
                        KernelAssert::DebugCheck(Index3 >= 0, "ConvexHullBuilder::Initialize: не найдена третья точка");
                        if (BestTriangleAreaSquared < MinimumTriangleAreaSquared)
                        {
                            Error = "Could not find a suitable initial triangle because its area was too small";
                            return Result::Degenerate;
                        }

                        // Ровно три вершины: два треугольника спина к спине
                        if (PositionCount == 3)
                        {
                            Face* T1 = CreateTriangle(Index1, Index2, Index3);
                            Face* T2 = CreateTriangle(Index1, Index3, Index2);

                            LinkFace(T1->FirstEdge, T2->FirstEdge->NextEdge->NextEdge);
                            LinkFace(T1->FirstEdge->NextEdge, T2->FirstEdge->NextEdge);
                            LinkFace(T1->FirstEdge->NextEdge->NextEdge, T2->FirstEdge);

                            return Result::Success;
                        }

                        // Точка, образующая наибольший тетраэдр
                        const Vector3 InitialPlaneNormal = (PositionsReference[static_cast<Size>(Index2)]
                            - PositionsReference[static_cast<Size>(Index1)])
                            .Cross(PositionsReference[static_cast<Size>(Index3)]
                                - PositionsReference[static_cast<Size>(Index1)]).Normalized();
                        const Vector3 InitialPlaneCentroid = (PositionsReference[static_cast<Size>(Index1)]
                            + PositionsReference[static_cast<Size>(Index2)]
                            + PositionsReference[static_cast<Size>(Index3)]) / 3.0f;
                        Int32 Index4 = -1;
                        Float MaxDistance = 0.0f;
                        for (Int32 Index = 0; Index < PositionCount; ++Index)
                        {
                            if (Index == Index1 || Index == Index2 || Index == Index3)
                                continue;
                            const Float Distance = (PositionsReference[static_cast<Size>(Index)] - InitialPlaneCentroid)
                                .Dot(InitialPlaneNormal);
                            if (Absolute(Distance) > Absolute(MaxDistance))
                            {
                                MaxDistance = Distance;
                                Index4 = Index;
                            }
                        }

                        // Оболочка плоская?
                        if (Square(MaxDistance) <= Square(CoplanarSlopFactor) * CoplanarToleranceSquared)
                        {
                            // Проецируем все точки в 2D
                            const Vector3 Base1 = InitialPlaneNormal.GetNormalizedPerpendicular();
                            const Vector3 Base2 = InitialPlaneNormal.Cross(Base1);
                            KernelContainers::Array<Vector3> Positions2D;
                            Positions2D.Reserve(PositionsReference.GetCount());
                            for (const Vector3& Position : PositionsReference)
                                Positions2D.Add(Vector3(Base1.Dot(Position), Base2.Dot(Position), 0.0f));

                            KernelContainers::Array<Int32> Edges2D;
                            Geometry2D::ConvexHullBuilder2D Builder2D(Positions2D);
                            const Geometry2D::ConvexHullBuilder2D::Result Result2D =
                                Builder2D.Initialize(Index1, Index2, Index3, MaxVertices, Tolerance, Edges2D);

                            // Две грани спина к спине
                            Face* F1 = CreateFace();
                            Face* F2 = CreateFace();

                            KernelContainers::Array<Edge*> EdgesF1;
                            EdgesF1.Reserve(Edges2D.GetCount());
                            for (const Int32 StartIndex : Edges2D)
                            {
                                Edge* EdgeValue = KernelMemory::Make<Edge>(F1, StartIndex);
                                if (EdgesF1.IsEmpty())
                                    F1->FirstEdge = EdgeValue;
                                else
                                    EdgesF1.Back()->NextEdge = EdgeValue;
                                EdgesF1.Add(EdgeValue);
                            }
                            EdgesF1.Back()->NextEdge = F1->FirstEdge;

                            KernelContainers::Array<Edge*> EdgesF2;
                            EdgesF2.Reserve(Edges2D.GetCount());
                            for (Int32 Index = static_cast<Int32>(Edges2D.GetCount()) - 1; Index >= 0; --Index)
                            {
                                Edge* EdgeValue = KernelMemory::Make<Edge>(F2, Edges2D[static_cast<Size>(Index)]);
                                if (EdgesF2.IsEmpty())
                                    F2->FirstEdge = EdgeValue;
                                else
                                    EdgesF2.Back()->NextEdge = EdgeValue;
                                EdgesF2.Add(EdgeValue);
                            }
                            EdgesF2.Back()->NextEdge = F2->FirstEdge;

                            for (Size Index = 0; Index < Edges2D.GetCount(); ++Index)
                                LinkFace(EdgesF1[Index], EdgesF2[(2 * Edges2D.GetCount() - 2 - Index) % Edges2D.GetCount()]);

                            F1->CalculateNormalAndCentroid(PositionsReference.begin());
                            F2->Normal = -F1->Normal;
                            F2->Centroid = F1->Centroid;

                            return Result2D == Geometry2D::ConvexHullBuilder2D::Result::MaxVerticesReached
                                ? Result::MaxVerticesReached : Result::Success;
                        }

                        // Нормали должны смотреть наружу
                        if (MaxDistance < 0.0f)
                        {
                            const Int32 Temporary = Index2;
                            Index2 = Index3;
                            Index3 = Temporary;
                        }

                        // Тетраэдр
                        Face* T1 = CreateTriangle(Index1, Index2, Index4);
                        Face* T2 = CreateTriangle(Index2, Index3, Index4);
                        Face* T3 = CreateTriangle(Index3, Index1, Index4);
                        Face* T4 = CreateTriangle(Index1, Index3, Index2);

                        LinkFace(T1->FirstEdge, T4->FirstEdge->NextEdge->NextEdge);
                        LinkFace(T1->FirstEdge->NextEdge, T2->FirstEdge->NextEdge->NextEdge);
                        LinkFace(T1->FirstEdge->NextEdge->NextEdge, T3->FirstEdge->NextEdge);
                        LinkFace(T2->FirstEdge, T4->FirstEdge->NextEdge);
                        LinkFace(T2->FirstEdge->NextEdge, T3->FirstEdge->NextEdge->NextEdge);
                        LinkFace(T3->FirstEdge, T4->FirstEdge);

                        // Начальные списки конфликтов
                        Faces InitialFaces;
                        InitialFaces.Add(T1);
                        InitialFaces.Add(T2);
                        InitialFaces.Add(T3);
                        InitialFaces.Add(T4);
                        for (Int32 Index = 0; Index < PositionCount; ++Index)
                            if (Index != Index1 && Index != Index2 && Index != Index3 && Index != Index4)
                                (void)AssignPointToFace(Index, InitialFaces, ToleranceSquared);

                        // Верхняя оценка числа используемых вершин (для ограничения MaxVertices)
                        Int32 NumVerticesUsed = 4;

                        for (;;)
                        {
                            // Грань с самой дальней точкой
                            Face* FaceWithFurthestPoint = nullptr;
                            Float FurthestDistanceSquared = 0.0f;
                            for (Face* FaceValue : HullFaces)
                                if (FaceValue->FurthestPointDistanceSquared > FurthestDistanceSquared)
                                {
                                    FurthestDistanceSquared = FaceValue->FurthestPointDistanceSquared;
                                    FaceWithFurthestPoint = FaceValue;
                                }

                            Int32 FurthestPointIndex = -1;
                            if (FaceWithFurthestPoint != nullptr)
                            {
                                FurthestPointIndex = FaceWithFurthestPoint->ConflictList.Back();
                                FaceWithFurthestPoint->ConflictList.PopBack();
                            }
                            else if (!CoplanarPositions.IsEmpty())
                            {
                                // Пытаемся заново распределить компланарные точки (пересчитывается расстояние до оболочки)
                                CoplanarList Coplanars;
                                Coplanars.Reserve(CoplanarPositions.GetCount());
                                for (const Coplanar& Item : CoplanarPositions)
                                    Coplanars.Add(Item);
                                CoplanarPositions.Clear();

                                KernelTypes::Bool Added = false;
                                for (const Coplanar& Item : Coplanars)
                                    Added |= AssignPointToFace(Item.PositionIndex, HullFaces, ToleranceSquared);

                                // Если что-то назначили, повторяем цикл, чтобы подобрать это
                                if (Added)
                                    continue;

                                // Список пуст: точек не осталось
                                if (CoplanarPositions.IsEmpty())
                                    break;

                                do
                                {
                                    // Вершина, самая далёкая от оболочки
                                    Size BestIndex = 0;
                                    Float BestDistanceSquared = CoplanarPositions[0].DistanceSquared;
                                    for (Size Index = 1; Index < CoplanarPositions.GetCount(); ++Index)
                                    {
                                        const Coplanar& Item = CoplanarPositions[Index];
                                        if (Item.DistanceSquared > BestDistanceSquared)
                                        {
                                            BestIndex = Index;
                                            BestDistanceSquared = Item.DistanceSquared;
                                        }
                                    }

                                    // В конец и удалить
                                    const Coplanar Temporary = CoplanarPositions[BestIndex];
                                    CoplanarPositions[BestIndex] = CoplanarPositions.Back();
                                    CoplanarPositions.Back() = Temporary;

                                    FurthestPointIndex = CoplanarPositions.Back().PositionIndex;
                                    CoplanarPositions.PopBack();

                                    // Грань, от которой точка дальше всего
                                    GetFaceForPoint(PositionsReference[static_cast<Size>(FurthestPointIndex)],
                                        HullFaces, FaceWithFurthestPoint, BestDistanceSquared);
                                } while (!CoplanarPositions.IsEmpty() && FaceWithFurthestPoint == nullptr);

                                if (FaceWithFurthestPoint == nullptr)
                                    break;
                            }
                            else
                            {
                                // Вершин больше нет
                                break;
                            }

                            // Ограничение на число вершин
                            if (NumVerticesUsed >= MaxVertices)
                            {
                                // Считаем реально используемые вершины (удаление вершин не учитывалось)
                                NumVerticesUsed = GetNumVerticesUsed();

                                if (NumVerticesUsed >= MaxVertices)
                                    return Result::MaxVerticesReached;
                            }

                            ++NumVerticesUsed;

                            // Добавляем точку в оболочку
                            Faces NewFaces;
                            AddPoint(FaceWithFurthestPoint, FurthestPointIndex, CoplanarToleranceSquared, NewFaces);

                            // Перераспределяем точки из списков конфликтов удалённых граней
                            for (const Face* FaceValue : HullFaces)
                                if (FaceValue->Removed)
                                    for (const Int32 Index : FaceValue->ConflictList)
                                        (void)AssignPointToFace(Index, NewFaces, ToleranceSquared);

                            // Окончательно удаляем грани, помеченные в AddPoint()
                            GarbageCollectFaces();
                        }

                        // Построение может не удасться, если точки почти компланарны
                        if (HullFaces.GetCount() < 2)
                        {
                            Error = "Too few faces in hull";
                            return Result::TooFewFaces;
                        }

                        return Result::Success;
                    }

                    void ConvexHullBuilder::AddPoint(
                        Face* FacingFace, Int32 Index, Float CoplanarToleranceSquared, Faces& OutNewFaces)
                    {
                        const Vector3 Position = PositionsReference[static_cast<Size>(Index)];

                        ValidateFaces();

                        // Ребро границы граней, не смотрящих на новую вершину
                        FullEdges Edges;
                        FindEdge(FacingFace, Position, Edges);
                        KernelAssert::DebugCheck(Edges.GetCount() >= 3, "ConvexHullBuilder::AddPoint: горизонт короче трёх рёбер");

                        // Новые грани
                        OutNewFaces.Reserve(Edges.GetCount());
                        for (const FullEdge& EdgeValue : Edges)
                        {
                            KernelAssert::DebugCheck(EdgeValue.StartIndex != EdgeValue.EndIndex,
                                "ConvexHullBuilder::AddPoint: вырожденное ребро горизонта");
                            Face* NewFace = CreateTriangle(EdgeValue.StartIndex, EdgeValue.EndIndex, Index);
                            OutNewFaces.Add(NewFace);
                        }

                        // Связываем рёбра
                        for (Size Edge = 0; Edge < OutNewFaces.GetCount(); ++Edge)
                        {
                            LinkFace(OutNewFaces[Edge]->FirstEdge, Edges[Edge].NeighbourEdge);
                            LinkFace(OutNewFaces[Edge]->FirstEdge->NextEdge,
                                OutNewFaces[(Edge + 1) % OutNewFaces.GetCount()]->FirstEdge->NextEdge->NextEdge);
                        }

                        // Обрабатываем изменённые грани, пока не останется непроверенных
                        Faces AffectedFaces;
                        for (Face* FaceValue : OutNewFaces)
                            AffectedFaces.Add(FaceValue);
                        while (!AffectedFaces.IsEmpty())
                        {
                            Face* FaceValue = AffectedFaces.Back();
                            AffectedFaces.PopBack();

                            if (!FaceValue->Removed)
                            {
                                // Сливаем с соседом, если грань вырождена
                                MergeDegenerateFace(FaceValue, AffectedFaces);

                                // Сливаем с компланарными соседями (или если сосед образует вогнутое ребро)
                                if (!FaceValue->Removed)
                                    MergeCoplanarOrConcaveFaces(FaceValue, CoplanarToleranceSquared, AffectedFaces);
                            }
                        }

                        ValidateFaces();
                    }

                    void ConvexHullBuilder::GarbageCollectFaces()
                    {
                        for (Int32 Index = static_cast<Int32>(HullFaces.GetCount()) - 1; Index >= 0; --Index)
                        {
                            Face* FaceValue = HullFaces[static_cast<Size>(Index)];
                            if (FaceValue->Removed)
                            {
                                FreeFace(FaceValue);
                                HullFaces.RemoveAt(static_cast<Size>(Index));
                            }
                        }
                    }

                    ConvexHullBuilder::Face* ConvexHullBuilder::CreateFace()
                    {
                        Face* FaceValue = KernelMemory::Make<Face>();
                        HullFaces.Add(FaceValue);
                        return FaceValue;
                    }

                    ConvexHullBuilder::Face* ConvexHullBuilder::CreateTriangle(Int32 Index1, Int32 Index2, Int32 Index3)
                    {
                        Face* FaceValue = CreateFace();
                        FaceValue->Initialize(Index1, Index2, Index3, PositionsReference.begin());
                        return FaceValue;
                    }

                    void ConvexHullBuilder::FreeFace(Face* FaceValue)
                    {
                        KernelAssert::DebugCheck(FaceValue->Removed, "ConvexHullBuilder::FreeFace: грань не помечена удалённой");

                        const Edge* EdgeValue = FaceValue->FirstEdge;
                        if (EdgeValue != nullptr)
                            do
                            {
                                KernelAssert::DebugCheck(EdgeValue->NeighbourEdge == nullptr,
                                    "ConvexHullBuilder::FreeFace: удаляемая грань всё ещё связана");
                                EdgeValue = EdgeValue->NextEdge;
                            } while (EdgeValue != FaceValue->FirstEdge);

                        KernelMemory::Destroy(FaceValue);
                    }

                    void ConvexHullBuilder::LinkFace(Edge* Edge1, Edge* Edge2) noexcept
                    {
                        KernelAssert::DebugCheck(Edge1->NeighbourEdge == nullptr, "ConvexHullBuilder::LinkFace: ребро 1 уже связано");
                        KernelAssert::DebugCheck(Edge2->NeighbourEdge == nullptr, "ConvexHullBuilder::LinkFace: ребро 2 уже связано");
                        KernelAssert::DebugCheck(Edge1->OwnerFace != Edge2->OwnerFace, "ConvexHullBuilder::LinkFace: рёбра одной грани");
                        KernelAssert::DebugCheck(Edge1->StartIndex == Edge2->NextEdge->StartIndex,
                            "ConvexHullBuilder::LinkFace: вершины рёбер не совпадают");
                        KernelAssert::DebugCheck(Edge2->StartIndex == Edge1->NextEdge->StartIndex,
                            "ConvexHullBuilder::LinkFace: вершины рёбер не совпадают");

                        Edge1->NeighbourEdge = Edge2;
                        Edge2->NeighbourEdge = Edge1;
                    }

                    void ConvexHullBuilder::UnlinkFace(Face* FaceValue) noexcept
                    {
                        Edge* EdgeValue = FaceValue->FirstEdge;
                        do
                        {
                            if (EdgeValue->NeighbourEdge != nullptr)
                            {
                                KernelAssert::DebugCheck(EdgeValue->NeighbourEdge->NeighbourEdge == EdgeValue,
                                    "ConvexHullBuilder::UnlinkFace: сосед не указывает на ребро");
                                EdgeValue->NeighbourEdge->NeighbourEdge = nullptr;
                                EdgeValue->NeighbourEdge = nullptr;
                            }
                            EdgeValue = EdgeValue->NextEdge;
                        } while (EdgeValue != FaceValue->FirstEdge);
                    }

                    void ConvexHullBuilder::FindEdge(Face* FacingFace, const Vector3& Vertex, FullEdges& OutEdges) const
                    {
                        KernelAssert::DebugCheck(OutEdges.IsEmpty(), "ConvexHullBuilder::FindEdge: ожидается пустой массив");
                        KernelAssert::DebugCheck(FacingFace->IsFacing(Vertex), "ConvexHullBuilder::FindEdge: грань не смотрит на вершину");

                        FacingFace->Removed = true;

                        // Вместо рекурсии используем собственный стек
                        struct StackEntry
                        {
                            Edge* FirstEdgeValue;
                            Edge* CurrentEdge;
                        };

                        constexpr Int32 MaxEdgeLength = 128;
                        StackEntry Stack[MaxEdgeLength];
                        Int32 CurrentStackPosition = 0;

                        static_assert(alignof(Edge) >= 2, "Нужен младший бит указателя, чтобы отличать завершение обхода");

                        // Начинаем с переданной грани; младший бит указателя отличает его от первого ребра
                        Stack[0].FirstEdgeValue = FacingFace->FirstEdge;
                        Stack[0].CurrentEdge = reinterpret_cast<Edge*>(
                            reinterpret_cast<std::uintptr_t>(FacingFace->FirstEdge) | 1);

                        for (;;)
                        {
                            StackEntry& CurrentEntry = Stack[CurrentStackPosition];

                            Edge* RawEdge = CurrentEntry.CurrentEdge;
                            Edge* EdgeValue = reinterpret_cast<Edge*>(
                                reinterpret_cast<std::uintptr_t>(RawEdge) & ~std::uintptr_t(1));
                            CurrentEntry.CurrentEdge = EdgeValue->NextEdge;

                            // Вернулись к первому ребру: грань пройдена
                            if (RawEdge == CurrentEntry.FirstEdgeValue)
                            {
                                // Грань удаляется: отвязываем её, освободит вызывающий
                                UnlinkFace(EdgeValue->OwnerFace);

                                if (--CurrentStackPosition < 0)
                                    break;
                            }
                            else
                            {
                                // Посещаем соседнюю грань
                                Edge* NeighbourEdgeValue = EdgeValue->NeighbourEdge;
                                if (NeighbourEdgeValue != nullptr)
                                {
                                    Face* Neighbour = NeighbourEdgeValue->OwnerFace;
                                    if (!Neighbour->Removed)
                                    {
                                        if (Neighbour->IsFacing(Vertex))
                                        {
                                            // Вершина спереди: грань удаляется
                                            Neighbour->Removed = true;

                                            ++CurrentStackPosition;
                                            KernelAssert::DebugCheck(CurrentStackPosition < MaxEdgeLength,
                                                "ConvexHullBuilder::FindEdge: переполнение стека");
                                            StackEntry& NewEntry = Stack[CurrentStackPosition];
                                            NewEntry.FirstEdgeValue = NeighbourEdgeValue;
                                            // Это ребро проверять повторно не нужно: мы пришли через него
                                            NewEntry.CurrentEdge = NeighbourEdgeValue->NextEdge;
                                        }
                                        else
                                        {
                                            // Вершина сзади: ребро сохраняем
                                            FullEdge Full;
                                            Full.NeighbourEdge = NeighbourEdgeValue;
                                            Full.StartIndex = EdgeValue->StartIndex;
                                            Full.EndIndex = NeighbourEdgeValue->StartIndex;
                                            OutEdges.Add(Full);
                                        }
                                    }
                                }
                            }
                        }

                        for (Size Index = 0; Index < OutEdges.GetCount(); ++Index)
                            KernelAssert::DebugCheck(OutEdges[Index].EndIndex == OutEdges[(Index + 1) % OutEdges.GetCount()].StartIndex,
                                "ConvexHullBuilder::FindEdge: горизонт не замкнут");
                    }

                    void ConvexHullBuilder::MergeFaces(Edge* EdgeValue)
                    {
                        Face* FaceValue = EdgeValue->OwnerFace;

                        Edge* NextEdgeValue = EdgeValue->NextEdge;
                        Edge* PreviousEdgeValue = EdgeValue->GetPreviousEdge();

                        Edge* OtherEdge = EdgeValue->NeighbourEdge;
                        Face* OtherFace = OtherEdge->OwnerFace;

                        KernelAssert::DebugCheck(FaceValue != OtherFace, "ConvexHullBuilder::MergeFaces: слияние грани с самой собой");

                        // Рёбра другой грани переходят во владение FaceValue
                        Edge* Current = OtherEdge->NextEdge;
                        PreviousEdgeValue->NextEdge = Current;
                        for (;;)
                        {
                            Current->OwnerFace = FaceValue;
                            if (Current->NextEdge == OtherEdge)
                            {
                                Current->NextEdge = NextEdgeValue;
                                break;
                            }
                            Current = Current->NextEdge;
                        }

                        // Если первое ребро - EdgeValue, его надо заменить: оно больше не часть грани.
                        // Заменяем первым ребром слитой грани, чтобы обход цикла вызывающего кода завершился.
                        if (FaceValue->FirstEdge == EdgeValue)
                            FaceValue->FirstEdge = PreviousEdgeValue->NextEdge;

                        KernelMemory::Destroy(EdgeValue);
                        KernelMemory::Destroy(OtherEdge);

                        OtherFace->FirstEdge = nullptr;
                        OtherFace->Removed = true;

                        FaceValue->CalculateNormalAndCentroid(PositionsReference.begin());

                        // Объединяем списки конфликтов
                        if (FaceValue->FurthestPointDistanceSquared > OtherFace->FurthestPointDistanceSquared)
                        {
                            // У этой грани точка дальше: она остаётся последней
                            if (FaceValue->ConflictList.IsEmpty())
                                FaceValue->ConflictList.Insert(
                                    FaceValue->ConflictList.end(), OtherFace->ConflictList.begin(),
                                    OtherFace->ConflictList.end());
                            else
                                FaceValue->ConflictList.Insert(
                                    FaceValue->ConflictList.end() - 1, OtherFace->ConflictList.begin(),
                                    OtherFace->ConflictList.end());
                        }
                        else
                        {
                            // Дальняя точка у другой грани: её список добавляем в конец
                            FaceValue->ConflictList.Insert(
                                FaceValue->ConflictList.end(), OtherFace->ConflictList.begin(),
                                OtherFace->ConflictList.end());
                            FaceValue->FurthestPointDistanceSquared = OtherFace->FurthestPointDistanceSquared;
                        }
                        OtherFace->ConflictList.Clear();
                    }

                    void ConvexHullBuilder::MergeDegenerateFace(Face* FaceValue, Faces& AffectedFaces)
                    {
                        // Проверяем площадь грани
                        if (FaceValue->Normal.LengthSq() < MinimumTriangleAreaSquared)
                        {
                            // Самое длинное ребро: для "занозы" это сохраняет выпуклость
                            Float MaxLengthSquared = 0.0f;
                            Edge* LongestEdge = nullptr;
                            Edge* EdgeValue = FaceValue->FirstEdge;
                            Vector3 P1 = PositionsReference[static_cast<Size>(EdgeValue->StartIndex)];
                            do
                            {
                                Edge* Next = EdgeValue->NextEdge;
                                const Vector3 P2 = PositionsReference[static_cast<Size>(Next->StartIndex)];
                                const Float LengthSquared = (P2 - P1).LengthSq();
                                if (LengthSquared >= MaxLengthSquared)
                                {
                                    MaxLengthSquared = LengthSquared;
                                    LongestEdge = EdgeValue;
                                }
                                P1 = P2;
                                EdgeValue = Next;
                            } while (EdgeValue != FaceValue->FirstEdge);

                            // Сливаем с гранью по самому длинному ребру
                            MergeFaces(LongestEdge);

                            // Убираем некорректные рёбра
                            RemoveInvalidEdges(FaceValue, AffectedFaces);
                        }
                    }

                    void ConvexHullBuilder::MergeCoplanarOrConcaveFaces(
                        Face* FaceValue, Float CoplanarToleranceSquared, Faces& AffectedFaces)
                    {
                        KernelTypes::Bool Merged = false;

                        Edge* EdgeValue = FaceValue->FirstEdge;
                        do
                        {
                            // Следующее ребро запоминаем заранее: это может быть удалено
                            Edge* NextEdgeValue = EdgeValue->NextEdge;

                            // Если центроид одной грани выше плоскости другой больше чем на допуск, сливаем
                            const Face* OtherFace = EdgeValue->NeighbourEdge->OwnerFace;
                            const Vector3 DeltaCentroid = OtherFace->Centroid - FaceValue->Centroid;
                            const Float DistanceOtherFaceCentroid = FaceValue->Normal.Dot(DeltaCentroid);
                            const Float SignedDistanceOtherFaceCentroidSquared =
                                Absolute(DistanceOtherFaceCentroid) * DistanceOtherFaceCentroid;
                            const Float DistanceFaceCentroid = -OtherFace->Normal.Dot(DeltaCentroid);
                            const Float SignedDistanceFaceCentroidSquared =
                                Absolute(DistanceFaceCentroid) * DistanceFaceCentroid;
                            const Float FaceNormalLengthSquared = FaceValue->Normal.LengthSq();
                            const Float OtherFaceNormalLengthSquared = OtherFace->Normal.LengthSq();
                            if ((SignedDistanceOtherFaceCentroidSquared > -CoplanarToleranceSquared * FaceNormalLengthSquared
                                    || SignedDistanceFaceCentroidSquared > -CoplanarToleranceSquared * OtherFaceNormalLengthSquared)
                                && FaceValue->Normal.Dot(OtherFace->Normal) > 0.0f) // Грани спина к спине не сливаем
                            {
                                MergeFaces(EdgeValue);
                                Merged = true;
                            }

                            EdgeValue = NextEdgeValue;
                        } while (EdgeValue != FaceValue->FirstEdge);

                        if (Merged)
                            RemoveInvalidEdges(FaceValue, AffectedFaces);
                    }

                    void ConvexHullBuilder::MarkAffected(Face* FaceValue, Faces& AffectedFaces)
                    {
                        if (AffectedFaces.Find(FaceValue) == nullptr)
                            AffectedFaces.Add(FaceValue);
                    }

                    void ConvexHullBuilder::RemoveInvalidEdges(Face* FaceValue, Faces& AffectedFaces)
                    {
                        // Плоскость пересчитывается один раз в конце, т.к. внутри она не используется
                        KernelTypes::Bool RecalculatePlane = false;

                        // Повторяем, пока удаляются рёбра
                        KernelTypes::Bool Removed;
                        do
                        {
                            Removed = false;

                            Edge* EdgeValue = FaceValue->FirstEdge;
                            Face* NeighbourFace = EdgeValue->NeighbourEdge->OwnerFace;
                            do
                            {
                                Edge* NextEdgeValue = EdgeValue->NextEdge;
                                Face* NextNeighbourFace = NextEdgeValue->NeighbourEdge->OwnerFace;

                                if (NeighbourFace == FaceValue)
                                {
                                    // Удаляем по одному ребру; если следующее ребро - сосед, это ребро ведёт назад
                                    // и является внутренним. Иначе идём дальше по ребру.
                                    if (EdgeValue->NeighbourEdge == NextEdgeValue)
                                    {
                                        Edge* PreviousEdgeValue = EdgeValue->GetPreviousEdge();
                                        PreviousEdgeValue->NextEdge = NextEdgeValue->NextEdge;
                                        if (FaceValue->FirstEdge == EdgeValue || FaceValue->FirstEdge == NextEdgeValue)
                                            FaceValue->FirstEdge = PreviousEdgeValue;

                                        KernelMemory::Destroy(EdgeValue);
                                        KernelMemory::Destroy(NextEdgeValue);

                                        // Осталось ли в грани только 2 ребра?
                                        if (RemoveTwoEdgeFace(FaceValue, AffectedFaces))
                                            return; // Грани больше нет

                                        RecalculatePlane = true;
                                        Removed = true;
                                        break;
                                    }
                                }
                                else if (NeighbourFace == NextNeighbourFace)
                                {
                                    // Два ребра ведут в одну грань: удаляем второе

                                    // Сначала объединяем рёбра соседа
                                    Edge* NeighbourEdgeValue = NextEdgeValue->NeighbourEdge;
                                    Edge* NextNeighbourEdge = NeighbourEdgeValue->NextEdge;
                                    if (NeighbourFace->FirstEdge == NextNeighbourEdge)
                                        NeighbourFace->FirstEdge = NeighbourEdgeValue;
                                    NeighbourEdgeValue->NextEdge = NextNeighbourEdge->NextEdge;
                                    NeighbourEdgeValue->NeighbourEdge = EdgeValue;
                                    KernelMemory::Destroy(NextNeighbourEdge);

                                    // Затем собственные рёбра
                                    if (FaceValue->FirstEdge == NextEdgeValue)
                                        FaceValue->FirstEdge = EdgeValue;
                                    EdgeValue->NextEdge = NextEdgeValue->NextEdge;
                                    EdgeValue->NeighbourEdge = NeighbourEdgeValue;
                                    KernelMemory::Destroy(NextEdgeValue);

                                    // У соседа осталось 2 ребра?
                                    if (!RemoveTwoEdgeFace(NeighbourFace, AffectedFaces))
                                    {
                                        // Нет: пересчитываем плоскость
                                        NeighbourFace->CalculateNormalAndCentroid(PositionsReference.begin());

                                        MarkAffected(NeighbourFace, AffectedFaces);
                                    }

                                    // У этой грани осталось 2 ребра?
                                    if (RemoveTwoEdgeFace(FaceValue, AffectedFaces))
                                        return; // Грани больше нет

                                    RecalculatePlane = true;
                                    Removed = true;
                                    break;
                                }

                                // Ребро в порядке, идём дальше
                                EdgeValue = NextEdgeValue;
                                NeighbourFace = NextNeighbourFace;
                            } while (EdgeValue != FaceValue->FirstEdge);
                        } while (Removed);

                        if (RecalculatePlane)
                            FaceValue->CalculateNormalAndCentroid(PositionsReference.begin());
                    }

                    KernelTypes::Bool ConvexHullBuilder::RemoveTwoEdgeFace(Face* FaceValue, Faces& AffectedFaces)
                    {
                        Edge* EdgeValue = FaceValue->FirstEdge;
                        Edge* NextEdgeValue = EdgeValue->NextEdge;

                        KernelAssert::DebugCheck(EdgeValue != NextEdgeValue, "ConvexHullBuilder::RemoveTwoEdgeFace: грань из одного ребра");

                        if (NextEdgeValue->NextEdge == EdgeValue)
                        {
                            // Обоих соседей надо перепроверить
                            Edge* NeighbourEdgeValue = EdgeValue->NeighbourEdge;
                            Face* NeighbourFace = NeighbourEdgeValue->OwnerFace;
                            Edge* NextNeighbourEdge = NextEdgeValue->NeighbourEdge;
                            Face* NextNeighbourFace = NextNeighbourEdge->OwnerFace;

                            MarkAffected(NeighbourFace, AffectedFaces);
                            MarkAffected(NextNeighbourFace, AffectedFaces);

                            // Связываем соседей друг с другом
                            NeighbourEdgeValue->NeighbourEdge = NextNeighbourEdge;
                            NextNeighbourEdge->NeighbourEdge = NeighbourEdgeValue;

                            // Отвязываем свои рёбра
                            EdgeValue->NeighbourEdge = nullptr;
                            NextEdgeValue->NeighbourEdge = nullptr;

                            FaceValue->Removed = true;
                            return true;
                        }

                        return false;
                    }

                    void ConvexHullBuilder::ValidateFace(const Face* FaceValue) const
                    {
                        if (FaceValue->Removed)
                        {
                            const Edge* EdgeValue = FaceValue->FirstEdge;
                            if (EdgeValue != nullptr)
                                do
                                {
                                    KernelAssert::DebugCheck(EdgeValue->NeighbourEdge == nullptr,
                                        "ConvexHullBuilder::ValidateFace: удалённая грань связана");
                                    EdgeValue = EdgeValue->NextEdge;
                                } while (EdgeValue != FaceValue->FirstEdge);
                        }
                        else
                        {
                            Int32 EdgeCount = 0;

                            const Edge* EdgeValue = FaceValue->FirstEdge;
                            do
                            {
                                ++EdgeCount;

                                // Соседние грани должны быть различны
                                if (HullFaces.GetCount() > 2)
                                    for (const Edge* OtherEdge = EdgeValue->NextEdge; OtherEdge != FaceValue->FirstEdge;
                                        OtherEdge = OtherEdge->NextEdge)
                                        KernelAssert::DebugCheck(
                                            EdgeValue->NeighbourEdge->OwnerFace != OtherEdge->NeighbourEdge->OwnerFace,
                                            "ConvexHullBuilder::ValidateFace: две связи с одной гранью");

                                KernelAssert::DebugCheck(EdgeValue->OwnerFace == FaceValue,
                                    "ConvexHullBuilder::ValidateFace: неверный владелец ребра");

                                const Edge* NeighbourEdgeValue = EdgeValue->NeighbourEdge;
                                KernelAssert::DebugCheck(NeighbourEdgeValue != nullptr,
                                    "ConvexHullBuilder::ValidateFace: у ребра нет соседа");
                                if (NeighbourEdgeValue != nullptr)
                                {
                                    KernelAssert::DebugCheck(NeighbourEdgeValue->NeighbourEdge == EdgeValue,
                                        "ConvexHullBuilder::ValidateFace: сосед указывает не на это ребро");
                                    KernelAssert::DebugCheck(NeighbourEdgeValue->OwnerFace != FaceValue,
                                        "ConvexHullBuilder::ValidateFace: сосед в той же грани");
                                    KernelAssert::DebugCheck(NeighbourEdgeValue->NextEdge->StartIndex == EdgeValue->StartIndex,
                                        "ConvexHullBuilder::ValidateFace: вершины соседа не совпадают");
                                    KernelAssert::DebugCheck(EdgeValue->NextEdge->StartIndex == NeighbourEdgeValue->StartIndex,
                                        "ConvexHullBuilder::ValidateFace: вершины соседа не совпадают");
                                }

                                EdgeValue = EdgeValue->NextEdge;
                            } while (EdgeValue != FaceValue->FirstEdge);

                            KernelAssert::DebugCheck(EdgeCount >= 3, "ConvexHullBuilder::ValidateFace: меньше трёх рёбер");
                        }
                    }

                    void ConvexHullBuilder::ValidateFaces() const
                    {
                        for (const Face* FaceValue : HullFaces)
                            ValidateFace(FaceValue);
                    }

                    void ConvexHullBuilder::GetCenterOfMassAndVolume(Vector3& CenterOfMass, Float& Volume) const
                    {
                        // Четвёртая точка: среднее центроидов всех граней
                        Vector3 V4 = Vector3::sZero();
                        for (const Face* FaceValue : HullFaces)
                            V4 += FaceValue->Centroid;
                        V4 /= static_cast<Float>(HullFaces.GetCount());

                        // Сумма по тетраэдрам
                        Volume = 0.0f;
                        CenterOfMass = Vector3::sZero();
                        for (const Face* FaceValue : HullFaces)
                        {
                            const Edge* EdgeValue = FaceValue->FirstEdge;
                            const Vector3 V1 = PositionsReference[static_cast<Size>(EdgeValue->StartIndex)];

                            EdgeValue = EdgeValue->NextEdge;
                            Vector3 V2 = PositionsReference[static_cast<Size>(EdgeValue->StartIndex)];

                            for (EdgeValue = EdgeValue->NextEdge; EdgeValue != FaceValue->FirstEdge; EdgeValue = EdgeValue->NextEdge)
                            {
                                const Vector3 V3 = PositionsReference[static_cast<Size>(EdgeValue->StartIndex)];

                                // https://en.wikipedia.org/wiki/Tetrahedron#Volume
                                // Деление на 6 и на 4 откладывается до конца цикла
                                const Float VolumeTetrahedron = (V1 - V4).Dot((V2 - V4).Cross(V3 - V4));
                                const Vector3 CenterOfMassTetrahedron = V1 + V2 + V3 + V4;

                                Volume += VolumeTetrahedron;
                                CenterOfMass += VolumeTetrahedron * CenterOfMassTetrahedron;

                                V2 = V3;
                            }
                        }

                        // Если объёма нет (всё в плоскости), центр масс - среднее
                        if (Volume > FloatEpsilon)
                            CenterOfMass /= 4.0f * Volume;
                        else
                            CenterOfMass = V4;
                        Volume /= 6.0f;
                    }

                    void ConvexHullBuilder::DetermineMaxError(
                        Face*& FaceWithMaxError,
                        Float& MaxError,
                        Int32& MaxErrorPositionIndex,
                        Float& CoplanarDistance
                    ) const
                    {
                        CoplanarDistance = DetermineCoplanarDistance();

                        // Расстояние от многоугольника до самой дальней точки снаружи оболочки
                        Float MaxErrorValue = 0.0f;
                        Face* MaxErrorFace = nullptr;
                        Int32 MaxErrorPoint = -1;

                        for (Int32 Index = 0; Index < static_cast<Int32>(PositionsReference.GetCount()); ++Index)
                        {
                            const Vector3 Point = PositionsReference[static_cast<Size>(Index)];

                            // Минимум по всем граням: возможно несколько почти компланарных граней, и по одной
                            // грани точка может оказаться снаружи многоугольника, хотя лежит внутри соседней.
                            Float MinEdgeDistanceSquared = FLT_MAX;
                            Face* MinEdgeDistanceFace = nullptr;
                            for (Face* FaceValue : HullFaces)
                            {
                                const Float NormalLength = FaceValue->Normal.Length();
                                KernelAssert::DebugCheck(NormalLength > 0.0f, "ConvexHullBuilder::DetermineMaxError: нулевая нормаль");
                                const Float PlaneDistance = FaceValue->Normal.Dot(Point - FaceValue->Centroid) / NormalLength;
                                if (PlaneDistance > -CoplanarSlopFactor * CoplanarDistance)
                                {
                                    const Float EdgeDistanceSquared = GetDistanceToEdgeSquared(Point, FaceValue);
                                    if (EdgeDistanceSquared < MinEdgeDistanceSquared)
                                    {
                                        MinEdgeDistanceSquared = EdgeDistanceSquared;
                                        MinEdgeDistanceFace = FaceValue;
                                    }

                                    // Точка внутри многоугольника и перед плоскостью: измеряем расстояние
                                    if (EdgeDistanceSquared == 0.0f && PlaneDistance > MaxErrorValue)
                                    {
                                        MaxErrorValue = PlaneDistance;
                                        MaxErrorFace = FaceValue;
                                        MaxErrorPoint = Index;
                                    }
                                }
                            }

                            // Если расстояние до ближайшего ребра больше текущей ошибки, берём его
                            const Float MinEdgeDistance = EngineKernel::KernelCompiler::SquareRoot(MinEdgeDistanceSquared);
                            if (MinEdgeDistanceFace != nullptr && MinEdgeDistance > MaxErrorValue)
                            {
                                MaxErrorValue = MinEdgeDistance;
                                MaxErrorFace = MinEdgeDistanceFace;
                                MaxErrorPoint = Index;
                            }
                        }

                        FaceWithMaxError = MaxErrorFace;
                        MaxError = MaxErrorValue;
                        MaxErrorPositionIndex = MaxErrorPoint;
                    }
                }
            }
        }
    }
}
