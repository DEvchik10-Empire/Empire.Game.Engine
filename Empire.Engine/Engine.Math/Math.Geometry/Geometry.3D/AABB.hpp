#pragma once

// Axis-aligned bounding box ported from Jolt Physics 5.7.0 (Geometry/AABox.h).
#include "Empire.Engine/Engine.Kernel/Kernel.Assert/Assert.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Compiler/Compiler.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Containers/Containers.Array.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Types/Types.hpp"
#include "Empire.Engine/Engine.Math/Math.Matrix/Matrix4.hpp"
#include "Empire.Engine/Engine.Math/Math.Vector/UVector4.hpp"
#include "Empire.Engine/Engine.Math/Math.Vector/Vector3.hpp"
#include "Empire.Engine/Engine.Math/Math.Geometry/Geometry.3D/Plane.hpp"
#include "Empire.Engine/Engine.Math/Math.Geometry/Geometry.3D/Sphere.hpp"
#include "Empire.Engine/Engine.Math/Math.Geometry/Geometry.3D/Triangle.hpp"
#include "Empire.Engine/Engine.Math/Math.Geometry/Geometry.3D/IndexedTriangle.hpp"
#include "Empire.Engine/Engine.Math/Math.Scalar/Scalar.hpp"

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
                    using namespace EngineKernel;
                    using namespace MathMatrix;
                    using namespace MathVector;

                    class [[nodiscard]] AABB
                    {
                    public:
                        AABB() noexcept { SetEmpty(); }
                        AABB(const Vector3& Minimum, const Vector3& Maximum) noexcept : mMin(Minimum), mMax(Maximum) {}
                        AABB(const Vector3& Center, KernelTypes::Float Radius) noexcept
                            : mMin(Center - Vector3::sReplicate(Radius))
                            , mMax(Center + Vector3::sReplicate(Radius))
                        {
                        }

                        [[nodiscard]] static AABB sFromTwoPoints(const Vector3& Point1, const Vector3& Point2) noexcept
                        {
                            return AABB(Vector3::sMin(Point1, Point2), Vector3::sMax(Point1, Point2));
                        }

                        [[nodiscard]] static AABB sFromTriangle(
                            const KernelContainers::Array<Float3>& Vertices, const IndexedTriangle& TriangleValue)
                        {
                            AABB Result = sFromTwoPoints(
                                Vector3::sLoadFloat3Unsafe(Vertices[TriangleValue.mIdx[0]]),
                                Vector3::sLoadFloat3Unsafe(Vertices[TriangleValue.mIdx[1]]));
                            Result.Encapsulate(Vector3::sLoadFloat3Unsafe(Vertices[TriangleValue.mIdx[2]]));
                            return Result;
                        }

                        [[nodiscard]] static AABB sBiggest() noexcept
                        {
                            constexpr KernelTypes::Float HalfMaximum = 1.7014117331926443e38f;
                            return AABB(Vector3::sReplicate(-HalfMaximum), Vector3::sReplicate(HalfMaximum));
                        }

                        [[nodiscard]] KernelTypes::Bool operator==(const AABB& Other) const noexcept
                        {
                            return mMin == Other.mMin && mMax == Other.mMax;
                        }

                        [[nodiscard]] KernelTypes::Bool operator!=(const AABB& Other) const noexcept
                        {
                            return !(*this == Other);
                        }

                        void SetEmpty() noexcept
                        {
                            const KernelTypes::Float Maximum = KernelCompiler::BitCast<KernelTypes::Float>(0x7f7fffffu);
                            mMin = Vector3::sReplicate(Maximum);
                            mMax = Vector3::sReplicate(-Maximum);
                        }

                        [[nodiscard]] KernelTypes::Bool IsValid() const noexcept
                        {
                            return mMin.GetX() <= mMax.GetX() && mMin.GetY() <= mMax.GetY() && mMin.GetZ() <= mMax.GetZ();
                        }

                        void Encapsulate(const Vector3& Position) noexcept
                        {
                            mMin = Vector3::sMin(mMin, Position);
                            mMax = Vector3::sMax(mMax, Position);
                        }

                        void Encapsulate(const AABB& Other) noexcept
                        {
                            mMin = Vector3::sMin(mMin, Other.mMin);
                            mMax = Vector3::sMax(mMax, Other.mMax);
                        }

                        void Encapsulate(const Triangle& Other) noexcept
                        {
                            for (const Float3& Vertex : Other.mVertices)
                                Encapsulate(Vector3::sLoadFloat3Unsafe(Vertex));
                        }

                        void Encapsulate(
                            const KernelContainers::Array<Float3>& Vertices, const IndexedTriangle& TriangleValue)
                        {
                            for (const KernelTypes::UInt32 Index : TriangleValue.mIdx)
                                Encapsulate(Vector3::sLoadFloat3Unsafe(Vertices[Index]));
                        }

                        [[nodiscard]] AABB Intersect(const AABB& Other) const noexcept
                        {
                            return AABB(Vector3::sMax(mMin, Other.mMin), Vector3::sMin(mMax, Other.mMax));
                        }

                        void EnsureMinimalEdgeLength(KernelTypes::Float MinimumEdgeLength) noexcept
                        {
                            const Vector3 MinimumLength = Vector3::sReplicate(MinimumEdgeLength);
                            mMax = Vector3::sSelect(mMax, mMin + MinimumLength,
                                Vector3::sLess(mMax - mMin, MinimumLength));
                        }

                        void ExpandBy(const Vector3& Amount) noexcept
                        {
                            mMin -= Amount;
                            mMax += Amount;
                        }

                        [[nodiscard]] Vector3 GetCenter() const noexcept { return 0.5f * (mMin + mMax); }
                        [[nodiscard]] Vector3 GetExtent() const noexcept { return 0.5f * (mMax - mMin); }
                        [[nodiscard]] Vector3 GetSize() const noexcept { return mMax - mMin; }

                        [[nodiscard]] KernelTypes::Float GetSurfaceArea() const noexcept
                        {
                            const Vector3 Extent = mMax - mMin;
                            return 2.0f * (Extent.GetX() * Extent.GetY()
                                + Extent.GetX() * Extent.GetZ() + Extent.GetY() * Extent.GetZ());
                        }

                        [[nodiscard]] KernelTypes::Float GetVolume() const noexcept
                        {
                            const Vector3 Extent = mMax - mMin;
                            return Extent.GetX() * Extent.GetY() * Extent.GetZ();
                        }

                        [[nodiscard]] KernelTypes::Bool Contains(const AABB& Other) const noexcept
                        {
                            return mMin.GetX() <= Other.mMin.GetX() && mMin.GetY() <= Other.mMin.GetY()
                                && mMin.GetZ() <= Other.mMin.GetZ() && mMax.GetX() >= Other.mMax.GetX()
                                && mMax.GetY() >= Other.mMax.GetY() && mMax.GetZ() >= Other.mMax.GetZ();
                        }

                        [[nodiscard]] KernelTypes::Bool Contains(const Vector3& Point) const noexcept
                        {
                            return mMin.GetX() <= Point.GetX() && mMin.GetY() <= Point.GetY()
                                && mMin.GetZ() <= Point.GetZ() && mMax.GetX() >= Point.GetX()
                                && mMax.GetY() >= Point.GetY() && mMax.GetZ() >= Point.GetZ();
                        }

                        [[nodiscard]] KernelTypes::Bool Overlaps(const AABB& Other) const noexcept
                        {
                            return mMin.GetX() <= Other.mMax.GetX() && mMin.GetY() <= Other.mMax.GetY()
                                && mMin.GetZ() <= Other.mMax.GetZ() && mMax.GetX() >= Other.mMin.GetX()
                                && mMax.GetY() >= Other.mMin.GetY() && mMax.GetZ() >= Other.mMin.GetZ();
                        }

                        [[nodiscard]] KernelTypes::Bool Overlaps(const Plane& Other) const noexcept
                        {
                            const Vector3 Normal = Other.GetNormal();
                            return Other.SignedDistance(GetSupport(Normal)) >= 0.0f
                                && Other.SignedDistance(GetSupport(-Normal)) <= 0.0f;
                        }

                        void Translate(const Vector3& Translation) noexcept
                        {
                            mMin += Translation;
                            mMax += Translation;
                        }

                        [[nodiscard]] AABB Transformed(const Matrix4& Matrix) const noexcept
                        {
                            Vector3 NewMinimum = Matrix.GetTranslation();
                            Vector3 NewMaximum = Matrix.GetTranslation();
                            for (KernelTypes::UInt32 Column = 0; Column < 3; ++Column)
                            {
                                const Vector3 Axis = Matrix.GetColumn3(Column);
                                const Vector3 A = Axis * mMin[Column];
                                const Vector3 B = Axis * mMax[Column];
                                NewMinimum += Vector3::sMin(A, B);
                                NewMaximum += Vector3::sMax(A, B);
                            }
                            return AABB(NewMinimum, NewMaximum);
                        }

                        [[nodiscard]] AABB Scaled(const Vector3& Scale) const noexcept
                        {
                            return sFromTwoPoints(mMin * Scale, mMax * Scale);
                        }

                        [[nodiscard]] Vector3 GetSupport(const Vector3& Direction) const noexcept
                        {
                            return Vector3::sSelect(mMax, mMin, Vector3::sLess(Direction, Vector3::sZero()));
                        }

                        template <typename VertexArray>
                        void GetSupportingFace(const Vector3& Direction, VertexArray& Vertices) const
                        {
                            Vertices.Resize(4);
                            const KernelTypes::Int32 Axis = Direction.Abs().GetHighestComponentIndex();
                            const KernelTypes::Bool Negative = Direction[static_cast<KernelTypes::UInt32>(Axis)] < 0.0f;
                            if (Axis == 0 && Negative)
                            {
                                Vertices[0] = Vector3(mMax.GetX(), mMin.GetY(), mMin.GetZ());
                                Vertices[1] = Vector3(mMax.GetX(), mMax.GetY(), mMin.GetZ());
                                Vertices[2] = Vector3(mMax.GetX(), mMax.GetY(), mMax.GetZ());
                                Vertices[3] = Vector3(mMax.GetX(), mMin.GetY(), mMax.GetZ());
                            }
                            else if (Axis == 0)
                            {
                                Vertices[0] = Vector3(mMin.GetX(), mMin.GetY(), mMin.GetZ());
                                Vertices[1] = Vector3(mMin.GetX(), mMin.GetY(), mMax.GetZ());
                                Vertices[2] = Vector3(mMin.GetX(), mMax.GetY(), mMax.GetZ());
                                Vertices[3] = Vector3(mMin.GetX(), mMax.GetY(), mMin.GetZ());
                            }
                            else if (Axis == 1 && Negative)
                            {
                                Vertices[0] = Vector3(mMin.GetX(), mMax.GetY(), mMin.GetZ());
                                Vertices[1] = Vector3(mMin.GetX(), mMax.GetY(), mMax.GetZ());
                                Vertices[2] = Vector3(mMax.GetX(), mMax.GetY(), mMax.GetZ());
                                Vertices[3] = Vector3(mMax.GetX(), mMax.GetY(), mMin.GetZ());
                            }
                            else if (Axis == 1)
                            {
                                Vertices[0] = Vector3(mMin.GetX(), mMin.GetY(), mMin.GetZ());
                                Vertices[1] = Vector3(mMax.GetX(), mMin.GetY(), mMin.GetZ());
                                Vertices[2] = Vector3(mMax.GetX(), mMin.GetY(), mMax.GetZ());
                                Vertices[3] = Vector3(mMin.GetX(), mMin.GetY(), mMax.GetZ());
                            }
                            else if (Negative)
                            {
                                Vertices[0] = Vector3(mMin.GetX(), mMin.GetY(), mMax.GetZ());
                                Vertices[1] = Vector3(mMax.GetX(), mMin.GetY(), mMax.GetZ());
                                Vertices[2] = Vector3(mMax.GetX(), mMax.GetY(), mMax.GetZ());
                                Vertices[3] = Vector3(mMin.GetX(), mMax.GetY(), mMax.GetZ());
                            }
                            else
                            {
                                Vertices[0] = Vector3(mMin.GetX(), mMin.GetY(), mMin.GetZ());
                                Vertices[1] = Vector3(mMin.GetX(), mMax.GetY(), mMin.GetZ());
                                Vertices[2] = Vector3(mMax.GetX(), mMax.GetY(), mMin.GetZ());
                                Vertices[3] = Vector3(mMax.GetX(), mMin.GetY(), mMin.GetZ());
                            }
                        }

                        [[nodiscard]] Vector3 GetClosestPoint(const Vector3& Point) const noexcept
                        {
                            return Vector3::sMin(Vector3::sMax(Point, mMin), mMax);
                        }

                        [[nodiscard]] KernelTypes::Float GetSqDistanceTo(const Vector3& Point) const noexcept
                        {
                            return (GetClosestPoint(Point) - Point).LengthSq();
                        }

                        Vector3 mMin;
                        Vector3 mMax;
                    };

                    inline KernelTypes::Bool Sphere::Overlaps(const AABB& Other) const noexcept
                    {
                        return Other.GetSqDistanceTo(GetCenter()) <= MathScalar::Square(mRadius);
                    }
                }
            }
        }
    }
}
