#pragma once

// Fixed-size dynamic-dimension vector ported from Jolt Physics 5.7.0 (Math/Vector.h).
#include "Empire.Engine/Engine.Kernel/Kernel.Assert/Assert.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Types/Types.hpp"
#include "Empire.Engine/Engine.Math/Math.Scalar/Scalar.hpp"

namespace DEvchik10Empire
{
    namespace EmpireGameEngine
    {
        namespace EmpireEngine
        {
            namespace EngineMath
            {
                namespace MathMatrix
                {
                    using namespace EngineKernel;
                    using namespace MathScalar;

                    template <KernelTypes::UInt32 Rows>
                    class [[nodiscard]] Vector
                    {
                    public:
                        constexpr Vector() noexcept = default;
                        constexpr Vector(const Vector&) noexcept = default;

                        [[nodiscard]] static constexpr KernelTypes::UInt32 GetRows() noexcept { return Rows; }

                        constexpr void SetZero() noexcept
                        {
                            for (KernelTypes::UInt32 Row = 0; Row < Rows; ++Row)
                                mValues[Row] = 0.0f;
                        }

                        [[nodiscard]] static constexpr Vector sZero() noexcept
                        {
                            Vector Result;
                            Result.SetZero();
                            return Result;
                        }

                        template <typename OtherVector>
                        constexpr void CopyPart(
                            const OtherVector& Source,
                            KernelTypes::UInt32 SourceRow,
                            KernelTypes::UInt32 RowCount,
                            KernelTypes::UInt32 DestinationRow
                        ) noexcept
                        {
                            KernelAssert::DebugCheck(DestinationRow + RowCount <= Rows, "Vector copy destination is in range");
                            for (KernelTypes::UInt32 Row = 0; Row < RowCount; ++Row)
                                mValues[DestinationRow + Row] = Source[SourceRow + Row];
                        }

                        [[nodiscard]] constexpr KernelTypes::Float operator[](KernelTypes::UInt32 Row) const noexcept
                        {
                            KernelAssert::DebugCheck(Row < Rows, "Vector row index is in range");
                            return mValues[Row];
                        }

                        constexpr KernelTypes::Float& operator[](KernelTypes::UInt32 Row) noexcept
                        {
                            KernelAssert::DebugCheck(Row < Rows, "Vector row index is in range");
                            return mValues[Row];
                        }

                        [[nodiscard]] constexpr KernelTypes::Bool operator==(const Vector& Other) const noexcept
                        {
                            for (KernelTypes::UInt32 Row = 0; Row < Rows; ++Row)
                                if (mValues[Row] != Other.mValues[Row])
                                    return false;
                            return true;
                        }

                        [[nodiscard]] constexpr KernelTypes::Bool operator!=(const Vector& Other) const noexcept
                        {
                            return !(*this == Other);
                        }

                        [[nodiscard]] constexpr KernelTypes::Bool IsZero() const noexcept
                        {
                            for (KernelTypes::UInt32 Row = 0; Row < Rows; ++Row)
                                if (mValues[Row] != 0.0f)
                                    return false;
                            return true;
                        }

                        [[nodiscard]] constexpr KernelTypes::Bool IsNearZero(KernelTypes::Float MaxDistanceSquared = 1.0e-12f) const noexcept
                        {
                            return LengthSq() <= MaxDistanceSquared;
                        }

                        [[nodiscard]] constexpr KernelTypes::Bool IsClose(
                            const Vector& Other,
                            KernelTypes::Float MaxDistanceSquared = 1.0e-12f
                        ) const noexcept
                        {
                            return (Other - *this).LengthSq() <= MaxDistanceSquared;
                        }

                        [[nodiscard]] constexpr Vector operator*(KernelTypes::Float Scale) const noexcept
                        {
                            Vector Result;
                            for (KernelTypes::UInt32 Row = 0; Row < Rows; ++Row)
                                Result.mValues[Row] = mValues[Row] * Scale;
                            return Result;
                        }

                        constexpr Vector& operator*=(KernelTypes::Float Scale) noexcept
                        {
                            for (KernelTypes::UInt32 Row = 0; Row < Rows; ++Row)
                                mValues[Row] *= Scale;
                            return *this;
                        }

                        [[nodiscard]] constexpr Vector operator/(KernelTypes::Float Scale) const noexcept
                        {
                            Vector Result;
                            for (KernelTypes::UInt32 Row = 0; Row < Rows; ++Row)
                                Result.mValues[Row] = mValues[Row] / Scale;
                            return Result;
                        }

                        constexpr Vector& operator/=(KernelTypes::Float Scale) noexcept
                        {
                            for (KernelTypes::UInt32 Row = 0; Row < Rows; ++Row)
                                mValues[Row] /= Scale;
                            return *this;
                        }

                        [[nodiscard]] constexpr Vector operator+(const Vector& Other) const noexcept
                        {
                            Vector Result;
                            for (KernelTypes::UInt32 Row = 0; Row < Rows; ++Row)
                                Result.mValues[Row] = mValues[Row] + Other.mValues[Row];
                            return Result;
                        }

                        constexpr Vector& operator+=(const Vector& Other) noexcept
                        {
                            for (KernelTypes::UInt32 Row = 0; Row < Rows; ++Row)
                                mValues[Row] += Other.mValues[Row];
                            return *this;
                        }

                        [[nodiscard]] constexpr Vector operator-() const noexcept
                        {
                            Vector Result;
                            for (KernelTypes::UInt32 Row = 0; Row < Rows; ++Row)
                                Result.mValues[Row] = -mValues[Row];
                            return Result;
                        }

                        [[nodiscard]] constexpr Vector operator-(const Vector& Other) const noexcept
                        {
                            Vector Result;
                            for (KernelTypes::UInt32 Row = 0; Row < Rows; ++Row)
                                Result.mValues[Row] = mValues[Row] - Other.mValues[Row];
                            return Result;
                        }

                        constexpr Vector& operator-=(const Vector& Other) noexcept
                        {
                            for (KernelTypes::UInt32 Row = 0; Row < Rows; ++Row)
                                mValues[Row] -= Other.mValues[Row];
                            return *this;
                        }

                        [[nodiscard]] constexpr KernelTypes::Float Dot(const Vector& Other) const noexcept
                        {
                            KernelTypes::Float Result = 0.0f;
                            for (KernelTypes::UInt32 Row = 0; Row < Rows; ++Row)
                                Result += mValues[Row] * Other.mValues[Row];
                            return Result;
                        }

                        [[nodiscard]] constexpr KernelTypes::Float LengthSq() const noexcept { return Dot(*this); }
                        [[nodiscard]] KernelTypes::Float Length() const noexcept { return Sqrt(LengthSq()); }

                        [[nodiscard]] constexpr KernelTypes::Bool IsNormalized(KernelTypes::Float ToleranceSquared = 1.0e-6f) const noexcept
                        {
                            return Abs(LengthSq() - 1.0f) <= ToleranceSquared;
                        }

                        [[nodiscard]] Vector Normalized() const noexcept { return *this / Length(); }

                        KernelTypes::Float mValues[Rows];
                    };

                    template <KernelTypes::UInt32 Rows>
                    [[nodiscard]] constexpr Vector<Rows> operator*(KernelTypes::Float Scale, const Vector<Rows>& Value) noexcept
                    {
                        return Value * Scale;
                    }
                }
            }
        }
    }
}
