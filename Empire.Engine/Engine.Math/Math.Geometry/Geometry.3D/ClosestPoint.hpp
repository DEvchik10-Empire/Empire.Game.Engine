#pragma once

#include "Empire.Engine/Engine.Kernel/Kernel.Types/Types.hpp"
#include "Empire.Engine/Engine.Math/Math.Scalar/Scalar.hpp"
#include "Empire.Engine/Engine.Math/Math.Vector/UVector4.hpp"
#include "Empire.Engine/Engine.Math/Math.Vector/Vector3.hpp"
#include "Empire.Engine/Engine.Math/Math.Vector/Vector4.hpp"

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
                    using namespace MathVector;

                    namespace ClosestPoint
                    {
                        inline constexpr KernelTypes::Float FloatEpsilon = 1.1920928955078125e-7f;

                        [[nodiscard]] inline KernelTypes::Bool GetBaryCentricCoordinates(
                            const Vector3& A, const Vector3& B,
                            KernelTypes::Float& U, KernelTypes::Float& V) noexcept
                        {
                            const Vector3 AB = B - A;
                            const KernelTypes::Float Denominator = AB.LengthSq();
                            if (Denominator < MathScalar::Square(FloatEpsilon))
                            {
                                if (A.LengthSq() < B.LengthSq())
                                {
                                    U = 1.0f;
                                    V = 0.0f;
                                }
                                else
                                {
                                    U = 0.0f;
                                    V = 1.0f;
                                }
                                return false;
                            }

                            V = -A.Dot(AB) / Denominator;
                            U = 1.0f - V;
                            return true;
                        }

                        [[nodiscard]] inline KernelTypes::Bool GetBaryCentricCoordinates(
                            const Vector3& A, const Vector3& B, const Vector3& C,
                            KernelTypes::Float& U, KernelTypes::Float& V, KernelTypes::Float& W) noexcept
                        {
                            const Vector3 V0 = B - A;
                            const Vector3 V1 = C - A;
                            const Vector3 V2 = C - B;
                            const KernelTypes::Float D00 = V0.LengthSq();
                            const KernelTypes::Float D11 = V1.LengthSq();
                            const KernelTypes::Float D22 = V2.LengthSq();
                            if (D00 <= D22)
                            {
                                const KernelTypes::Float D01 = V0.Dot(V1);
                                const KernelTypes::Float Denominator =
                                    MathScalar::DifferenceOfProducts(D00, D11, D01, D01);
                                if (Denominator <= 1.0e-6f * D00 * D11)
                                {
                                    if (D00 > D11)
                                    {
                                        static_cast<void>(GetBaryCentricCoordinates(A, B, U, V));
                                        W = 0.0f;
                                    }
                                    else
                                    {
                                        static_cast<void>(GetBaryCentricCoordinates(A, C, U, W));
                                        V = 0.0f;
                                    }
                                    return false;
                                }

                                const KernelTypes::Float A0 = A.Dot(V0);
                                const KernelTypes::Float A1 = A.Dot(V1);
                                V = MathScalar::DifferenceOfProducts(D01, A1, D11, A0) / Denominator;
                                W = MathScalar::DifferenceOfProducts(D01, A0, D00, A1) / Denominator;
                                U = 1.0f - V - W;
                            }
                            else
                            {
                                const KernelTypes::Float D12 = V1.Dot(V2);
                                const KernelTypes::Float Denominator =
                                    MathScalar::DifferenceOfProducts(D11, D22, D12, D12);
                                if (Denominator <= 1.0e-6f * D11 * D22)
                                {
                                    if (D11 > D22)
                                    {
                                        static_cast<void>(GetBaryCentricCoordinates(A, C, U, W));
                                        V = 0.0f;
                                    }
                                    else
                                    {
                                        static_cast<void>(GetBaryCentricCoordinates(B, C, V, W));
                                        U = 0.0f;
                                    }
                                    return false;
                                }

                                const KernelTypes::Float C1 = C.Dot(V1);
                                const KernelTypes::Float C2 = C.Dot(V2);
                                U = MathScalar::DifferenceOfProducts(D22, C1, D12, C2) / Denominator;
                                V = MathScalar::DifferenceOfProducts(D11, C2, D12, C1) / Denominator;
                                W = 1.0f - U - V;
                            }
                            return true;
                        }

                        [[nodiscard]] inline Vector3 GetClosestPointOnLine(
                            const Vector3& A, const Vector3& B, KernelTypes::UInt32& FeatureSet) noexcept
                        {
                            KernelTypes::Float U;
                            KernelTypes::Float V;
                            static_cast<void>(GetBaryCentricCoordinates(A, B, U, V));
                            if (V <= 0.0f)
                            {
                                FeatureSet = 0b0001;
                                return A;
                            }
                            if (U <= 0.0f)
                            {
                                FeatureSet = 0b0010;
                                return B;
                            }
                            FeatureSet = 0b0011;
                            return U * A + V * B;
                        }

                        template <KernelTypes::Bool MustIncludeC = false>
                        [[nodiscard]] inline Vector3 GetClosestPointOnTriangle(
                            const Vector3& InA, const Vector3& B, const Vector3& InC,
                            KernelTypes::UInt32& FeatureSet) noexcept
                        {
                            const KernelTypes::Bool SwapAC = (InC - B).LengthSq() < (InA - B).LengthSq();
                            const Vector3 A = SwapAC ? InC : InA;
                            const Vector3 C = SwapAC ? InA : InC;
                            Vector3 AB = B - A;
                            const Vector3 AC = C - A;
                            const Vector3 Normal = AB.CrossPrecise(AC);
                            const KernelTypes::Float NormalLengthSquared = Normal.LengthSq();
                            if (NormalLengthSquared < 1.0e-10f)
                            {
                                KernelTypes::UInt32 ClosestSet = 0b0100;
                                Vector3 ClosestPoint = InC;
                                KernelTypes::Float BestDistanceSquared = InC.LengthSq();
                                if constexpr (!MustIncludeC)
                                {
                                    const KernelTypes::Float DistanceA = InA.LengthSq();
                                    if (DistanceA < BestDistanceSquared)
                                    {
                                        ClosestSet = 0b0001;
                                        ClosestPoint = InA;
                                        BestDistanceSquared = DistanceA;
                                    }
                                    const KernelTypes::Float DistanceB = B.LengthSq();
                                    if (DistanceB < BestDistanceSquared)
                                    {
                                        ClosestSet = 0b0010;
                                        ClosestPoint = B;
                                        BestDistanceSquared = DistanceB;
                                    }
                                }

                                const KernelTypes::Float ACLengthSquared = AC.LengthSq();
                                if (ACLengthSquared > MathScalar::Square(FloatEpsilon))
                                {
                                    const KernelTypes::Float T = MathScalar::Clamp(-A.Dot(AC) / ACLengthSquared, 0.0f, 1.0f);
                                    const Vector3 Point = A + T * AC;
                                    const KernelTypes::Float Distance = Point.LengthSq();
                                    if (Distance < BestDistanceSquared)
                                    {
                                        ClosestSet = 0b0101;
                                        ClosestPoint = Point;
                                        BestDistanceSquared = Distance;
                                    }
                                }

                                const Vector3 BC = InC - B;
                                const KernelTypes::Float BCLengthSquared = BC.LengthSq();
                                if (BCLengthSquared > MathScalar::Square(FloatEpsilon))
                                {
                                    const KernelTypes::Float T = MathScalar::Clamp(-B.Dot(BC) / BCLengthSquared, 0.0f, 1.0f);
                                    const Vector3 Point = B + T * BC;
                                    const KernelTypes::Float Distance = Point.LengthSq();
                                    if (Distance < BestDistanceSquared)
                                    {
                                        ClosestSet = 0b0110;
                                        ClosestPoint = Point;
                                        BestDistanceSquared = Distance;
                                    }
                                }

                                if constexpr (!MustIncludeC)
                                {
                                    AB = B - InA;
                                    const KernelTypes::Float ABLengthSquared = AB.LengthSq();
                                    if (ABLengthSquared > MathScalar::Square(FloatEpsilon))
                                    {
                                        const KernelTypes::Float T = MathScalar::Clamp(-InA.Dot(AB) / ABLengthSquared, 0.0f, 1.0f);
                                        const Vector3 Point = InA + T * AB;
                                        const KernelTypes::Float Distance = Point.LengthSq();
                                        if (Distance < BestDistanceSquared)
                                        {
                                            ClosestSet = 0b0011;
                                            ClosestPoint = Point;
                                        }
                                    }
                                }

                                FeatureSet = ClosestSet;
                                return ClosestPoint;
                            }

                            const Vector3 AP = -A;
                            const KernelTypes::Float D1 = AB.Dot(AP);
                            const KernelTypes::Float D2 = AC.Dot(AP);
                            if (D1 <= 0.0f && D2 <= 0.0f)
                            {
                                FeatureSet = SwapAC ? 0b0100 : 0b0001;
                                return A;
                            }

                            const Vector3 BP = -B;
                            const KernelTypes::Float D3 = AB.Dot(BP);
                            const KernelTypes::Float D4 = AC.Dot(BP);
                            if (D3 >= 0.0f && D4 <= D3)
                            {
                                FeatureSet = 0b0010;
                                return B;
                            }
                            if (D1 * D4 <= D3 * D2 && D1 >= 0.0f && D3 <= 0.0f)
                            {
                                const KernelTypes::Float V = D1 / (D1 - D3);
                                FeatureSet = SwapAC ? 0b0110 : 0b0011;
                                return A + V * AB;
                            }

                            const Vector3 CP = -C;
                            const KernelTypes::Float D5 = AB.Dot(CP);
                            const KernelTypes::Float D6 = AC.Dot(CP);
                            if (D6 >= 0.0f && D5 <= D6)
                            {
                                FeatureSet = SwapAC ? 0b0001 : 0b0100;
                                return C;
                            }
                            if (D5 * D2 <= D1 * D6 && D2 >= 0.0f && D6 <= 0.0f)
                            {
                                const KernelTypes::Float W = D2 / (D2 - D6);
                                FeatureSet = 0b0101;
                                return A + W * AC;
                            }

                            const KernelTypes::Float D4MinusD3 = D4 - D3;
                            const KernelTypes::Float D5MinusD6 = D5 - D6;
                            if (D3 * D6 <= D5 * D4 && D4MinusD3 >= 0.0f && D5MinusD6 >= 0.0f)
                            {
                                const KernelTypes::Float W = D4MinusD3 / (D4MinusD3 + D5MinusD6);
                                FeatureSet = SwapAC ? 0b0011 : 0b0110;
                                return B + W * (C - B);
                            }

                            FeatureSet = 0b0111;
                            return Normal * (A + B + C).Dot(Normal) / (3.0f * NormalLengthSquared);
                        }

                        [[nodiscard]] inline KernelTypes::Bool OriginOutsideOfPlane(
                            const Vector3& A, const Vector3& B, const Vector3& C, const Vector3& D) noexcept
                        {
                            const Vector3 Normal = (B - A).CrossPrecise(C - A);
                            const KernelTypes::Float SignOrigin = A.Dot(Normal);
                            const KernelTypes::Float SignOpposite = (D - A).Dot(Normal);
                            return SignOrigin * SignOpposite > -FloatEpsilon;
                        }

                        [[nodiscard]] inline UVector4 OriginOutsideOfTetrahedronPlanes(
                            const Vector3& A, const Vector3& B, const Vector3& C, const Vector3& D) noexcept
                        {
                            const Vector3 AB = B - A;
                            const Vector3 AC = C - A;
                            const Vector3 AD = D - A;
                            const Vector3 BD = D - B;
                            const Vector3 BC = C - B;
                            const Vector3 ABC = AB.CrossPrecise(AC);
                            const Vector3 ACD = AC.CrossPrecise(AD);
                            const Vector3 ADB = AD.CrossPrecise(AB);
                            const Vector3 BDC = BD.CrossPrecise(BC);
                            const Vector4 SignOrigin(
                                A.Dot(ABC), A.Dot(ACD), A.Dot(ADB), B.Dot(BDC));
                            const Vector4 SignOpposite(
                                AD.Dot(ABC), AB.Dot(ACD), AC.Dot(ADB), -AB.Dot(BDC));

                            const KernelTypes::Bool AllPositive =
                                SignOpposite.GetX() >= 0.0f && SignOpposite.GetY() >= 0.0f
                                && SignOpposite.GetZ() >= 0.0f && SignOpposite.GetW() >= 0.0f;
                            const KernelTypes::Bool AllNegative =
                                SignOpposite.GetX() < 0.0f && SignOpposite.GetY() < 0.0f
                                && SignOpposite.GetZ() < 0.0f && SignOpposite.GetW() < 0.0f;
                            if (!AllPositive && !AllNegative)
                                return UVector4::sReplicate(0xffffffffu);

                            const KernelTypes::Float Tolerance = AllPositive ? -FloatEpsilon : FloatEpsilon;
                            const KernelTypes::Bool OutsideX = AllPositive
                                ? SignOrigin.GetX() >= Tolerance : SignOrigin.GetX() <= Tolerance;
                            const KernelTypes::Bool OutsideY = AllPositive
                                ? SignOrigin.GetY() >= Tolerance : SignOrigin.GetY() <= Tolerance;
                            const KernelTypes::Bool OutsideZ = AllPositive
                                ? SignOrigin.GetZ() >= Tolerance : SignOrigin.GetZ() <= Tolerance;
                            const KernelTypes::Bool OutsideW = AllPositive
                                ? SignOrigin.GetW() >= Tolerance : SignOrigin.GetW() <= Tolerance;
                            return UVector4(OutsideX ? 0xffffffffu : 0u,
                                OutsideY ? 0xffffffffu : 0u, OutsideZ ? 0xffffffffu : 0u,
                                OutsideW ? 0xffffffffu : 0u);
                        }

                        template <KernelTypes::Bool MustIncludeD = false>
                        [[nodiscard]] inline Vector3 GetClosestPointOnTetrahedron(
                            const Vector3& A, const Vector3& B, const Vector3& C, const Vector3& D,
                            KernelTypes::UInt32& FeatureSet) noexcept
                        {
                            KernelTypes::UInt32 ClosestSet = 0b1111;
                            Vector3 ClosestPoint = Vector3::sZero();
                            KernelTypes::Float BestDistanceSquared =
                                KernelCompiler::BitCast<KernelTypes::Float>(0x7f7fffffu);
                            const UVector4 Outside = OriginOutsideOfTetrahedronPlanes(A, B, C, D);

                            if (Outside.GetX())
                            {
                                if constexpr (MustIncludeD)
                                {
                                    ClosestSet = 0b0001;
                                    ClosestPoint = A;
                                }
                                else
                                    ClosestPoint = GetClosestPointOnTriangle(A, B, C, ClosestSet);
                                BestDistanceSquared = ClosestPoint.LengthSq();
                            }

                            if (Outside.GetY())
                            {
                                KernelTypes::UInt32 Set = 0;
                                const Vector3 Point = GetClosestPointOnTriangle<MustIncludeD>(A, C, D, Set);
                                const KernelTypes::Float Distance = Point.LengthSq();
                                if (Distance < BestDistanceSquared)
                                {
                                    BestDistanceSquared = Distance;
                                    ClosestPoint = Point;
                                    ClosestSet = (Set & 0b0001) + ((Set & 0b0110) << 1);
                                }
                            }

                            if (Outside.GetZ())
                            {
                                KernelTypes::UInt32 Set = 0;
                                const Vector3 Point = GetClosestPointOnTriangle<MustIncludeD>(A, B, D, Set);
                                const KernelTypes::Float Distance = Point.LengthSq();
                                if (Distance < BestDistanceSquared)
                                {
                                    BestDistanceSquared = Distance;
                                    ClosestPoint = Point;
                                    ClosestSet = (Set & 0b0011) + ((Set & 0b0100) << 1);
                                }
                            }

                            if (Outside.GetW())
                            {
                                KernelTypes::UInt32 Set = 0;
                                const Vector3 Point = GetClosestPointOnTriangle<MustIncludeD>(B, C, D, Set);
                                const KernelTypes::Float Distance = Point.LengthSq();
                                if (Distance < BestDistanceSquared)
                                {
                                    ClosestPoint = Point;
                                    ClosestSet = Set << 1;
                                }
                            }

                            FeatureSet = ClosestSet;
                            return ClosestPoint;
                        }
                    }
                }
            }
        }
    }
}
