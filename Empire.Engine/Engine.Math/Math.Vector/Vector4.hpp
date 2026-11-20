#pragma once

#include "Empire.Engine/Engine.Kernel/Kernel.Assert/Assert.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Compiler/Compiler.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Compiler/Compiler.Simd.Float4.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Types/Types.hpp"
#include "Empire.Engine/Engine.Math/Math.Vector/Float4.hpp"
#include "Empire.Engine/Engine.Math/Math.Vector/Swizzle.hpp"
#include "Empire.Engine/Engine.Math/Math.Vector/Vector3.hpp"

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

                    struct alignas(16) Vector4
                    {
                        KernelTypes::Float X;
                        KernelTypes::Float Y;
                        KernelTypes::Float Z;
                        KernelTypes::Float W;

                        constexpr Vector4() noexcept
                            : X(0.0f), Y(0.0f), Z(0.0f), W(0.0f)
                        {
                        }

                        constexpr Vector4(KernelTypes::Float X, KernelTypes::Float Y, KernelTypes::Float Z, KernelTypes::Float W) noexcept
                            : X(X), Y(Y), Z(Z), W(W)
                        {
                        }

                        explicit constexpr Vector4(const Vector3& Value) noexcept
                            : X(Value.GetX()), Y(Value.GetY()), Z(Value.GetZ()), W(Value.GetZ())
                        {
                        }

                        constexpr Vector4(const Vector3& Value, KernelTypes::Float WValue) noexcept
                            : X(Value.GetX()), Y(Value.GetY()), Z(Value.GetZ()), W(WValue)
                        {
                        }

                        [[nodiscard]] static constexpr Vector4 sZero() noexcept { return {}; }
                        [[nodiscard]] static constexpr Vector4 sOne() noexcept { return {1.0f, 1.0f, 1.0f, 1.0f}; }
                        [[nodiscard]] static constexpr Vector4 sReplicate(KernelTypes::Float Value) noexcept
                        {
                            return {Value, Value, Value, Value};
                        }
                        [[nodiscard]] static Vector4 sNaN() noexcept;
                        [[nodiscard]] static Vector4 sLoadFloat4(const Float4* Value) noexcept;
                        [[nodiscard]] static Vector4 sLoadFloat4Aligned(const Float4* Value) noexcept;

                        [[nodiscard]] static Vector4 sGatherFloat4Scaled(
                            const KernelTypes::Float* Base, const UVector4& Offsets, KernelTypes::Int32 Scale
                        ) noexcept;

                        template <KernelTypes::Int32 Scale>
                        [[nodiscard]] static Vector4 sGatherFloat4(const KernelTypes::Float* Base, const UVector4& Offsets) noexcept
                        {
                            static_assert(Scale > 0);
                            return sGatherFloat4Scaled(Base, Offsets, Scale);
                        }

                        [[nodiscard]] static Vector4 sMin(const Vector4& A, const Vector4& B) noexcept;
                        [[nodiscard]] static Vector4 sMax(const Vector4& A, const Vector4& B) noexcept;
                        [[nodiscard]] static Vector4 sClamp(const Vector4& Value, const Vector4& Minimum, const Vector4& Maximum) noexcept;
                        [[nodiscard]] static UVector4 sEquals(const Vector4& A, const Vector4& B) noexcept;
                        [[nodiscard]] static UVector4 sLess(const Vector4& A, const Vector4& B) noexcept;
                        [[nodiscard]] static UVector4 sLessOrEqual(const Vector4& A, const Vector4& B) noexcept;
                        [[nodiscard]] static UVector4 sGreater(const Vector4& A, const Vector4& B) noexcept;
                        [[nodiscard]] static UVector4 sGreaterOrEqual(const Vector4& A, const Vector4& B) noexcept;
                        [[nodiscard]] static Vector4 sFusedMultiplyAdd(const Vector4& A, const Vector4& B, const Vector4& Addend) noexcept;
                        [[nodiscard]] static Vector4 sSelect(const Vector4& NotSet, const Vector4& Set, const UVector4& Control) noexcept;
                        [[nodiscard]] static Vector4 sOr(const Vector4& A, const Vector4& B) noexcept;
                        [[nodiscard]] static Vector4 sXor(const Vector4& A, const Vector4& B) noexcept;
                        [[nodiscard]] static Vector4 sAnd(const Vector4& A, const Vector4& B) noexcept;
                        static void sSort4(Vector4& Values, UVector4& Indices) noexcept;
                        static void sSort4Reverse(Vector4& Values, UVector4& Indices) noexcept;

                        [[nodiscard]] static constexpr Vector4 Zero() noexcept { return {}; }
                        [[nodiscard]] static constexpr Vector4 One() noexcept { return {1.0f, 1.0f, 1.0f, 1.0f}; }
                        [[nodiscard]] static constexpr Vector4 Replicate(KernelTypes::Float Value) noexcept { return {Value, Value, Value, Value}; }

                        [[nodiscard]] constexpr KernelTypes::Float GetX() const noexcept { return X; }
                        [[nodiscard]] constexpr KernelTypes::Float GetY() const noexcept { return Y; }
                        [[nodiscard]] constexpr KernelTypes::Float GetZ() const noexcept { return Z; }
                        [[nodiscard]] constexpr KernelTypes::Float GetW() const noexcept { return W; }
                        constexpr void SetX(KernelTypes::Float Value) noexcept { X = Value; }
                        constexpr void SetY(KernelTypes::Float Value) noexcept { Y = Value; }
                        constexpr void SetZ(KernelTypes::Float Value) noexcept { Z = Value; }
                        constexpr void SetW(KernelTypes::Float Value) noexcept { W = Value; }
                        constexpr void Set(KernelTypes::Float XValue, KernelTypes::Float YValue, KernelTypes::Float ZValue, KernelTypes::Float WValue) noexcept
                        {
                            X = XValue;
                            Y = YValue;
                            Z = ZValue;
                            W = WValue;
                        }

                        [[nodiscard]] KernelTypes::Float operator[](KernelTypes::UInt32 Index) const noexcept
                        {
                            KernelAssert::DebugCheck(Index < 4, "Index < 4", "Vector4 index out of range");
                            switch (Index)
                            {
                                case 0: return X;
                                case 1: return Y;
                                case 2: return Z;
                                case 3: return W;
                                default: return X;
                            }
                        }

                        KernelTypes::Float& operator[](KernelTypes::UInt32 Index) noexcept
                        {
                            KernelAssert::DebugCheck(Index < 4, "Index < 4", "Vector4 index out of range");
                            switch (Index)
                            {
                                case 0: return X;
                                case 1: return Y;
                                case 2: return Z;
                                case 3: return W;
                                default: return X;
                            }
                        }

                        [[nodiscard]] KernelTypes::Bool IsClose(const Vector4& Other, KernelTypes::Float MaxDistanceSquared = 1.0e-12f) const noexcept;
                        [[nodiscard]] KernelTypes::Bool IsNearZero(KernelTypes::Float MaxDistanceSquared = 1.0e-12f) const noexcept;
                        [[nodiscard]] KernelTypes::Bool IsNormalized(KernelTypes::Float Tolerance = 1.0e-6f) const noexcept;
                        [[nodiscard]] KernelTypes::Bool IsNaN() const noexcept;

                        [[nodiscard]]
                        KernelTypes::Float Length() const noexcept;

                        [[nodiscard]]
                        KernelTypes::Float LengthSquared() const noexcept;

                        [[nodiscard]]
                        Vector4 Normalized() const noexcept;

                        [[nodiscard]]
                        constexpr KernelTypes::Float Dot(const Vector4& Other) const noexcept
                        {
                            return KernelCompiler::Simd::Float4::Load(X, Y, Z, W)
                                .Multiply(KernelCompiler::Simd::Float4::Load(Other.X, Other.Y, Other.Z, Other.W))
                                .ReduceSum();
                        }

                        [[nodiscard]]
                        KernelTypes::Float Distance(const Vector4& Other) const noexcept;

                        [[nodiscard]]
                        KernelTypes::Float DistanceSquared(const Vector4& Other) const noexcept;

                        [[nodiscard]] Vector4 operator*(const Vector4& Other) const noexcept
                        {
                            const KernelCompiler::Simd::Float4 Result =
                                KernelCompiler::Simd::Float4::Load(X, Y, Z, W).Multiply(
                                    KernelCompiler::Simd::Float4::Load(Other.X, Other.Y, Other.Z, Other.W));
                            return {
                                Result.Get<0>(),
                                Result.Get<1>(),
                                Result.Get<2>(),
                                Result.Get<3>()
                            };
                        }

                        constexpr Vector4 operator+(const Vector4& Other) const noexcept
                        {
                            const KernelCompiler::Simd::Float4 Result =
                                KernelCompiler::Simd::Float4::Load(X, Y, Z, W).Add(
                                    KernelCompiler::Simd::Float4::Load(Other.X, Other.Y, Other.Z, Other.W));
                            return {
                                Result.Get<0>(),
                                Result.Get<1>(),
                                Result.Get<2>(),
                                Result.Get<3>()
                            };
                        }

                        constexpr Vector4 operator-(const Vector4& Other) const noexcept
                        {
                            const KernelCompiler::Simd::Float4 Result =
                                KernelCompiler::Simd::Float4::Load(X, Y, Z, W).Subtract(
                                    KernelCompiler::Simd::Float4::Load(Other.X, Other.Y, Other.Z, Other.W));
                            return {
                                Result.Get<0>(),
                                Result.Get<1>(),
                                Result.Get<2>(),
                                Result.Get<3>()
                            };
                        }

                        constexpr Vector4 operator*(KernelTypes::Float Scalar) const noexcept
                        {
                            const KernelCompiler::Simd::Float4 Result =
                                KernelCompiler::Simd::Float4::Load(X, Y, Z, W).Multiply(
                                    KernelCompiler::Simd::Float4::Load(Scalar, Scalar, Scalar, Scalar));
                            return {
                                Result.Get<0>(),
                                Result.Get<1>(),
                                Result.Get<2>(),
                                Result.Get<3>()
                            };
                        }

                        constexpr Vector4 operator/(KernelTypes::Float Scalar) const noexcept
                        {
                            return {
                                X / Scalar,
                                Y / Scalar,
                                Z / Scalar,
                                W / Scalar
                            };
                        }

                        [[nodiscard]] Vector4 operator/(const Vector4& Other) const noexcept;

                        constexpr Vector4 operator-() const noexcept
                        {
                            return {
                                -X,
                                -Y,
                                -Z,
                                -W
                            };
                        }

                        constexpr Vector4& operator+=(const Vector4& Other) noexcept
                        {
                            X += Other.X;
                            Y += Other.Y;
                            Z += Other.Z;
                            W += Other.W;

                            return *this;
                        }

                        Vector4& operator*=(const Vector4& Other) noexcept;
                        Vector4& operator/=(const Vector4& Other) noexcept;

                        constexpr Vector4& operator-=(const Vector4& Other) noexcept
                        {
                            X -= Other.X;
                            Y -= Other.Y;
                            Z -= Other.Z;
                            W -= Other.W;

                            return *this;
                        }

                        constexpr Vector4& operator*=(KernelTypes::Float Scalar) noexcept
                        {
                            X *= Scalar;
                            Y *= Scalar;
                            Z *= Scalar;
                            W *= Scalar;

                            return *this;
                        }

                        constexpr Vector4& operator/=(KernelTypes::Float Scalar) noexcept
                        {
                            X /= Scalar;
                            Y /= Scalar;
                            Z /= Scalar;
                            W /= Scalar;

                            return *this;
                        }

                        constexpr KernelTypes::Bool operator==(const Vector4& Other) const noexcept
                        {
                            return X == Other.X &&
                                   Y == Other.Y &&
                                   Z == Other.Z &&
                                   W == Other.W;
                        }

                        constexpr KernelTypes::Bool operator!=(const Vector4& Other) const noexcept
                        {
                            return !(*this == Other);
                        }

                        template <KernelTypes::UInt32 SwizzleX, KernelTypes::UInt32 SwizzleY,
                                  KernelTypes::UInt32 SwizzleZ, KernelTypes::UInt32 SwizzleW>
                        [[nodiscard]] constexpr Vector4 Swizzle() const noexcept
                        {
                            static_assert(SwizzleX < 4 && SwizzleY < 4 && SwizzleZ < 4 && SwizzleW < 4);
                            return {(*this)[SwizzleX], (*this)[SwizzleY], (*this)[SwizzleZ], (*this)[SwizzleW]};
                        }

                        [[nodiscard]] constexpr Vector4 SplatX() const noexcept { return sReplicate(X); }
                        [[nodiscard]] constexpr Vector4 SplatY() const noexcept { return sReplicate(Y); }
                        [[nodiscard]] constexpr Vector4 SplatZ() const noexcept { return sReplicate(Z); }
                        [[nodiscard]] constexpr Vector4 SplatW() const noexcept { return sReplicate(W); }
                        [[nodiscard]] constexpr Vector3 SplatX3() const noexcept { return {X, X, X}; }
                        [[nodiscard]] constexpr Vector3 SplatY3() const noexcept { return {Y, Y, Y}; }
                        [[nodiscard]] constexpr Vector3 SplatZ3() const noexcept { return {Z, Z, Z}; }
                        [[nodiscard]] constexpr Vector3 SplatW3() const noexcept { return {W, W, W}; }
                        [[nodiscard]] KernelTypes::Int32 GetLowestComponentIndex() const noexcept;
                        [[nodiscard]] KernelTypes::Int32 GetHighestComponentIndex() const noexcept;
                        [[nodiscard]] Vector4 Abs() const noexcept;
                        [[nodiscard]] Vector4 Reciprocal() const noexcept;
                        [[nodiscard]] static Vector4 sDifferenceOfProducts(
                            const Vector4& A, const Vector4& B, const Vector4& C, const Vector4& D
                        ) noexcept;
                        [[nodiscard]] Vector4 DotV(const Vector4& Other) const noexcept;
                        [[nodiscard]] UVector4 ToInt() const noexcept;
                        [[nodiscard]] UVector4 ReinterpretAsInt() const noexcept;
                        [[nodiscard]] KernelTypes::Int32 GetSignBits() const noexcept;
                        [[nodiscard]] KernelTypes::Float ReduceMin() const noexcept;
                        [[nodiscard]] KernelTypes::Float ReduceMax() const noexcept;
                        [[nodiscard]] KernelTypes::Float ReduceSum() const noexcept;
                        [[nodiscard]] Vector4 Sqrt() const noexcept;
                        [[nodiscard]] Vector4 GetSign() const noexcept;
                        void SinCos(Vector4& Sine, Vector4& Cosine) const noexcept;
                        [[nodiscard]] Vector4 Tan() const noexcept;
                        [[nodiscard]] Vector4 ASin() const noexcept;
                        [[nodiscard]] Vector4 ACos() const noexcept;
                        [[nodiscard]] Vector4 ATan() const noexcept;
                        [[nodiscard]] static Vector4 sATan2(const Vector4& Y, const Vector4& X) noexcept;
                        [[nodiscard]] KernelTypes::UInt32 CompressUnitVector() const noexcept;
                        [[nodiscard]] static Vector4 sDecompressUnitVector(KernelTypes::UInt32 Value) noexcept;

                        template <KernelTypes::Int32 SignX, KernelTypes::Int32 SignY,
                                  KernelTypes::Int32 SignZ, KernelTypes::Int32 SignW>
                        [[nodiscard]] constexpr Vector4 FlipSign() const noexcept
                        {
                            static_assert((SignX == -1 || SignX == 1) && (SignY == -1 || SignY == 1)
                                && (SignZ == -1 || SignZ == 1) && (SignW == -1 || SignW == 1));
                            return {
                                KernelCompiler::BitCast<KernelTypes::Float>(
                                    KernelCompiler::BitCast<KernelTypes::UInt32>(X) ^ (SignX < 0 ? 0x80000000u : 0u)),
                                KernelCompiler::BitCast<KernelTypes::Float>(
                                    KernelCompiler::BitCast<KernelTypes::UInt32>(Y) ^ (SignY < 0 ? 0x80000000u : 0u)),
                                KernelCompiler::BitCast<KernelTypes::Float>(
                                    KernelCompiler::BitCast<KernelTypes::UInt32>(Z) ^ (SignZ < 0 ? 0x80000000u : 0u)),
                                KernelCompiler::BitCast<KernelTypes::Float>(
                                    KernelCompiler::BitCast<KernelTypes::UInt32>(W) ^ (SignW < 0 ? 0x80000000u : 0u))
                            };
                        }

                        void StoreFloat4(Float4* Output) const noexcept;
                    };

                    static_assert(sizeof(Vector4) == 16);
                    static_assert(alignof(Vector4) == 16);

                    constexpr Vector4 operator*(KernelTypes::Float Scalar, const Vector4& Vector) noexcept
                    {
                        return Vector * Scalar;
                    }
                }
            }
        }
    }
}
