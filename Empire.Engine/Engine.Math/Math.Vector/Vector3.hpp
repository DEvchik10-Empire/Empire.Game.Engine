#pragma once

#include "Empire.Engine/Engine.Kernel/Kernel.Assert/Assert.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Compiler/Compiler.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Compiler/Compiler.Simd.Float4.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Containers/Containers.StaticArray.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Types/Types.hpp"
#include "Empire.Engine/Engine.Math/Math.Scalar/Scalar.hpp"
#include "Empire.Engine/Engine.Math/Math.Vector/Float3.hpp"
#include "Empire.Engine/Engine.Math/Math.Vector/Swizzle.hpp"

namespace DEvchik10Empire
{
    namespace EmpireGameEngine
    {
        namespace EmpireEngine
        {
            namespace EngineMath
            {
                namespace MathVector
                {
                    using namespace EngineKernel;

                    struct UVector4;
                    struct Vector4;

                    struct alignas(16) Vector3
                    {
                        explicit constexpr Vector3(const Float3& Value) noexcept
                            : mX(Value.X), mY(Value.Y), mZ(Value.Z), mW(Value.Z)
                        {
                        }

                        explicit Vector3(const Vector4& Value) noexcept;

                        constexpr Vector3() noexcept
                            : mX(0.0f), mY(0.0f), mZ(0.0f), mW(0.0f)
                        {
                        }

                        constexpr Vector3(KernelTypes::Float X, KernelTypes::Float Y, KernelTypes::Float Z) noexcept
                            : mX(X), mY(Y), mZ(Z), mW(Z)
                        {
                        }

                        [[nodiscard]] static constexpr Vector3 sZero() noexcept { return {}; }
                        [[nodiscard]] static constexpr Vector3 sOne() noexcept { return {1.0f, 1.0f, 1.0f}; }
                        [[nodiscard]] static Vector3 sNaN() noexcept;
                        [[nodiscard]] static constexpr Vector3 sAxisX() noexcept { return {1.0f, 0.0f, 0.0f}; }
                        [[nodiscard]] static constexpr Vector3 sAxisY() noexcept { return {0.0f, 1.0f, 0.0f}; }
                        [[nodiscard]] static constexpr Vector3 sAxisZ() noexcept { return {0.0f, 0.0f, 1.0f}; }
                        [[nodiscard]] static constexpr Vector3 sReplicate(KernelTypes::Float Value) noexcept { return {Value, Value, Value}; }
                        [[nodiscard]] static Vector3 sLoadFloat3Unsafe(const Float3& Value) noexcept;
                        [[nodiscard]] static Vector3 sMin(const Vector3& A, const Vector3& B) noexcept;
                        [[nodiscard]] static Vector3 sMax(const Vector3& A, const Vector3& B) noexcept;
                        [[nodiscard]] static Vector3 sClamp(const Vector3& Value, const Vector3& Minimum, const Vector3& Maximum) noexcept;
                        [[nodiscard]] static UVector4 sEquals(const Vector3& A, const Vector3& B) noexcept;
                        [[nodiscard]] static UVector4 sLess(const Vector3& A, const Vector3& B) noexcept;
                        [[nodiscard]] static UVector4 sLessOrEqual(const Vector3& A, const Vector3& B) noexcept;
                        [[nodiscard]] static UVector4 sGreater(const Vector3& A, const Vector3& B) noexcept;
                        [[nodiscard]] static UVector4 sGreaterOrEqual(const Vector3& A, const Vector3& B) noexcept;
                        [[nodiscard]] static Vector3 sFusedMultiplyAdd(const Vector3& A, const Vector3& B, const Vector3& Addend) noexcept;
                        [[nodiscard]] static Vector3 sSelect(const Vector3& NotSet, const Vector3& Set, const UVector4& Control) noexcept;
                        [[nodiscard]] static Vector3 sOr(const Vector3& A, const Vector3& B) noexcept;
                        [[nodiscard]] static Vector3 sXor(const Vector3& A, const Vector3& B) noexcept;
                        [[nodiscard]] static Vector3 sAnd(const Vector3& A, const Vector3& B) noexcept;
                        [[nodiscard]] static Vector3 sUnitSpherical(KernelTypes::Float Theta, KernelTypes::Float Phi) noexcept;
                        [[nodiscard]] static const KernelContainers::StaticArray<Vector3, 1026>& sUnitSphere();

                        template <typename Random>
                        [[nodiscard]] static Vector3 sRandom(Random& Generator)
                        {
                            const auto RandomUnit = [&Generator]() noexcept
                            {
                                return static_cast<KernelTypes::Float>(Generator() - Generator.min())
                                    / static_cast<KernelTypes::Float>(Generator.max() - Generator.min());
                            };
                            const KernelTypes::Float Z = -1.0f + 2.0f * RandomUnit();
                            const KernelTypes::Float Theta = MathScalar::TwoPi * RandomUnit();
                            return sRandomFromZTheta(Z, Theta);
                        }

                        [[nodiscard]] constexpr KernelTypes::Float GetX() const noexcept { return mX; }
                        [[nodiscard]] constexpr KernelTypes::Float GetY() const noexcept { return mY; }
                        [[nodiscard]] constexpr KernelTypes::Float GetZ() const noexcept { return mZ; }
                        constexpr void SetX(KernelTypes::Float Value) noexcept { mX = Value; }
                        constexpr void SetY(KernelTypes::Float Value) noexcept { mY = Value; }
                        constexpr void SetZ(KernelTypes::Float Value) noexcept { mZ = mW = Value; }
                        constexpr void Set(KernelTypes::Float X, KernelTypes::Float Y, KernelTypes::Float Z) noexcept
                        {
                            mX = X;
                            mY = Y;
                            mZ = mW = Z;
                        }
                        [[nodiscard]] constexpr KernelTypes::Float operator[](KernelTypes::UInt32 Index) const noexcept
                        {
                            KernelAssert::DebugCheck(Index < 3, "Index < 3", "Vector3 index out of range");
                            return Index == 0 ? mX : (Index == 1 ? mY : mZ);
                        }
                        constexpr void SetComponent(KernelTypes::UInt32 Index, KernelTypes::Float Value) noexcept
                        {
                            KernelAssert::DebugCheck(Index < 3, "Index < 3", "Vector3 index out of range");
                            if (Index == 0)
                                mX = Value;
                            else if (Index == 1)
                                mY = Value;
                            else
                                mZ = mW = Value;
                        }

                        [[nodiscard]] KernelTypes::Bool IsClose(const Vector3& Other, KernelTypes::Float MaxDistSq = 1.0e-12f) const noexcept;
                        [[nodiscard]] KernelTypes::Bool IsNearZero(KernelTypes::Float MaxDistSq = 1.0e-12f) const noexcept;
                        [[nodiscard]] KernelTypes::Bool IsNormalized(KernelTypes::Float Tolerance = 1.0e-6f) const noexcept;
                        [[nodiscard]] KernelTypes::Bool IsNaN() const noexcept;

                        [[nodiscard]] constexpr KernelTypes::Bool operator==(const Vector3& Other) const noexcept
                        {
                            return mX == Other.mX && mY == Other.mY && mZ == Other.mZ;
                        }
                        [[nodiscard]] constexpr KernelTypes::Bool operator!=(const Vector3& Other) const noexcept { return !(*this == Other); }
                        [[nodiscard]] constexpr Vector3 operator*(const Vector3& Other) const noexcept
                        {
                            const KernelCompiler::Simd::Float4 Result =
                                KernelCompiler::Simd::Float4::Load(mX, mY, mZ, mW)
                                    .Multiply(KernelCompiler::Simd::Float4::Load(Other.mX, Other.mY, Other.mZ, Other.mW));
                            return {Result.Get<0>(), Result.Get<1>(), Result.Get<2>()};
                        }
                        [[nodiscard]] constexpr Vector3 operator*(KernelTypes::Float Scalar) const noexcept
                        {
                            const KernelCompiler::Simd::Float4 Result =
                                KernelCompiler::Simd::Float4::Load(mX, mY, mZ, mW)
                                    .Multiply(KernelCompiler::Simd::Float4::Load(Scalar, Scalar, Scalar, Scalar));
                            return {Result.Get<0>(), Result.Get<1>(), Result.Get<2>()};
                        }
                        [[nodiscard]] constexpr Vector3 operator/(KernelTypes::Float Scalar) const noexcept
                        {
                            return {mX / Scalar, mY / Scalar, mZ / Scalar};
                        }
                        [[nodiscard]] Vector3 operator/(const Vector3& Other) const noexcept;
                        constexpr Vector3& operator*=(const Vector3& Other) noexcept
                        {
                            mX *= Other.mX;
                            mY *= Other.mY;
                            mZ = mW = mZ * Other.mZ;
                            return *this;
                        }
                        constexpr Vector3& operator*=(KernelTypes::Float Scalar) noexcept
                        {
                            mX *= Scalar;
                            mY *= Scalar;
                            mZ = mW = mZ * Scalar;
                            return *this;
                        }
                        constexpr Vector3& operator/=(KernelTypes::Float Scalar) noexcept
                        {
                            mX /= Scalar;
                            mY /= Scalar;
                            mZ = mW = mZ / Scalar;
                            return *this;
                        }
                        [[nodiscard]] constexpr Vector3 operator+(const Vector3& Other) const noexcept
                        {
                            const KernelCompiler::Simd::Float4 Result =
                                KernelCompiler::Simd::Float4::Load(mX, mY, mZ, mW)
                                    .Add(KernelCompiler::Simd::Float4::Load(Other.mX, Other.mY, Other.mZ, Other.mW));
                            return {Result.Get<0>(), Result.Get<1>(), Result.Get<2>()};
                        }
                        constexpr Vector3& operator+=(const Vector3& Other) noexcept
                        {
                            mX += Other.mX;
                            mY += Other.mY;
                            mZ = mW = mZ + Other.mZ;
                            return *this;
                        }
                        [[nodiscard]] constexpr Vector3 operator-() const noexcept { return {-mX, -mY, -mZ}; }
                        [[nodiscard]] constexpr Vector3 operator-(const Vector3& Other) const noexcept
                        {
                            const KernelCompiler::Simd::Float4 Result =
                                KernelCompiler::Simd::Float4::Load(mX, mY, mZ, mW)
                                    .Subtract(KernelCompiler::Simd::Float4::Load(Other.mX, Other.mY, Other.mZ, Other.mW));
                            return {Result.Get<0>(), Result.Get<1>(), Result.Get<2>()};
                        }
                        constexpr Vector3& operator-=(const Vector3& Other) noexcept
                        {
                            mX -= Other.mX;
                            mY -= Other.mY;
                            mZ = mW = mZ - Other.mZ;
                            return *this;
                        }

                        template <KernelTypes::UInt32 X, KernelTypes::UInt32 Y, KernelTypes::UInt32 Z>
                        [[nodiscard]] constexpr Vector3 Swizzle() const noexcept
                        {
                            static_assert(X < 3 && Y < 3 && Z < 3, "Vector3 swizzle component must be X, Y, or Z");
                            return {(*this)[X], (*this)[Y], (*this)[Z]};
                        }
                        [[nodiscard]] constexpr Vector3 SplatX() const noexcept { return sReplicate(mX); }
                        [[nodiscard]] constexpr Vector3 SplatY() const noexcept { return sReplicate(mY); }
                        [[nodiscard]] constexpr Vector3 SplatZ() const noexcept { return sReplicate(mZ); }
                        [[nodiscard]] KernelTypes::Int32 GetLowestComponentIndex() const noexcept;
                        [[nodiscard]] KernelTypes::Int32 GetHighestComponentIndex() const noexcept;
                        [[nodiscard]] Vector3 Abs() const noexcept;
                        [[nodiscard]] Vector3 Reciprocal() const noexcept;
                        [[nodiscard]] static Vector3 sDifferenceOfProducts(
                            const Vector3& A, const Vector3& B, const Vector3& C, const Vector3& D
                        ) noexcept;
                        [[nodiscard]] Vector3 Cross(const Vector3& Other) const noexcept;
                        [[nodiscard]] Vector3 CrossPrecise(const Vector3& Other) const noexcept;
                        [[nodiscard]] Vector3 DotV(const Vector3& Other) const noexcept;
                        [[nodiscard]] Vector4 DotV4(const Vector3& Other) const noexcept;
                        [[nodiscard]] KernelTypes::Float Dot(const Vector3& Other) const noexcept;
                        [[nodiscard]] KernelTypes::Float LengthSq() const noexcept;
                        [[nodiscard]] KernelTypes::Float Length() const noexcept;
                        [[nodiscard]] Vector3 Normalized() const noexcept;
                        [[nodiscard]] Vector3 NormalizedOr(const Vector3& ZeroValue) const noexcept;
                        constexpr void StoreFloat3(Float3* Output) const noexcept
                        {
                            Output->X = mX;
                            Output->Y = mY;
                            Output->Z = mZ;
                        }
                        [[nodiscard]] UVector4 ToInt() const noexcept;
                        [[nodiscard]] UVector4 ReinterpretAsInt() const noexcept;
                        [[nodiscard]] KernelTypes::Float ReduceMin() const noexcept;
                        [[nodiscard]] KernelTypes::Float ReduceMax() const noexcept;
                        [[nodiscard]] KernelTypes::Float ReduceSum() const noexcept;
                        [[nodiscard]] Vector3 Sqrt() const noexcept;
                        [[nodiscard]] Vector3 GetNormalizedPerpendicular() const noexcept;
                        [[nodiscard]] Vector3 GetSign() const noexcept;

                        template <KernelTypes::Int32 X, KernelTypes::Int32 Y, KernelTypes::Int32 Z>
                        [[nodiscard]] Vector3 FlipSign() const noexcept
                        {
                            static_assert((X == 1 || X == -1) && (Y == 1 || Y == -1) && (Z == 1 || Z == -1));
                            return {mX * static_cast<KernelTypes::Float>(X),
                                    mY * static_cast<KernelTypes::Float>(Y),
                                    mZ * static_cast<KernelTypes::Float>(Z)};
                        }

                        [[nodiscard]] KernelTypes::UInt32 CompressUnitVector() const noexcept;
                        [[nodiscard]] static Vector3 sDecompressUnitVector(KernelTypes::UInt32 Value) noexcept;

                        [[nodiscard]] KernelTypes::Float LengthSquared() const noexcept { return LengthSq(); }
                        [[nodiscard]] KernelTypes::Float Distance(const Vector3& Other) const noexcept { return (*this - Other).Length(); }
                        [[nodiscard]] KernelTypes::Float DistanceSquared(const Vector3& Other) const noexcept { return (*this - Other).LengthSq(); }

                    private:
                        [[nodiscard]] static Vector3 sRandomFromZTheta(
                            KernelTypes::Float Z, KernelTypes::Float Theta) noexcept;

                        KernelTypes::Float mX;
                        KernelTypes::Float mY;
                        KernelTypes::Float mZ;
                        KernelTypes::Float mW;
                    };

                    static_assert(sizeof(Vector3) == 16);
                    static_assert(alignof(Vector3) == 16);

                    [[nodiscard]] constexpr Vector3 operator*(KernelTypes::Float Scalar, const Vector3& Vector) noexcept
                    {
                        return Vector * Scalar;
                    }
                }
            }
        }
    }
}
